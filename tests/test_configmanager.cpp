#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>

#include <cstdio>
#include <fstream>
#include <string>
#include <nlohmann/json.hpp>

#include "ConfigManagerBase.h"

// Parses a config file straight from disk so the test can inspect the raw JSON
// types that ConfigManagerBase wrote (configJson is a protected member).
static nlohmann::json readRawJson(const std::string& path)
{
    std::ifstream file(path);
    if (!file)
        return nlohmann::json{};
    return nlohmann::json::parse(file);
}

TEST_CASE("loadConfigFile reads keys of each JSON type", "[ConfigManager]")
{
    const std::string path = "og_config_test.json";

    {
        std::ofstream file(path);
        file << "{\n"
             << "  \"level\": \"level_map.json\",\n"
             << "  \"bots\": 3,\n"
             << "  \"bot_speed_mult\": 0.75,\n"
             << "  \"max_bullets\": 10,\n"
             << "  \"server_port\": 5000,\n"
             << "  \"v_sync\": true\n"
             << "}\n";
    }

    ConfigManagerBase::loadConfigFile(path);

    CHECK(ConfigManagerBase::readConfig("level") == "level_map.json");
    CHECK(ConfigManagerBase::readConfig("bots") == "3");
    CHECK(ConfigManagerBase::readConfig("bot_speed_mult") == "0.75");
    CHECK(ConfigManagerBase::readConfig("max_bullets") == "10");
    CHECK(ConfigManagerBase::readConfig("server_port") == "5000");
    // Booleans are surfaced as "1"/"0" strings.
    CHECK(ConfigManagerBase::readConfig("v_sync") == "1");

    // Unknown keys return an empty string.
    CHECK(ConfigManagerBase::readConfig("does_not_exist") == "");

    std::remove(path.c_str());
}

TEST_CASE("loadConfigFile with an empty JSON object yields no keys", "[ConfigManager]")
{
    const std::string path = "og_config_empty.json";

    {
        std::ofstream file(path);
        file << "{}\n";
    }

    ConfigManagerBase::loadConfigFile(path);
    CHECK(ConfigManagerBase::readConfig("level") == "");

    std::remove(path.c_str());
}

TEST_CASE("writeConfig bool overload stores a JSON boolean", "[ConfigManager]")
{
    const std::string path = "og_config_write.json";
    ConfigManagerBase::loadConfigFile(path);

    ConfigManagerBase::writeConfig("v_sync", true);

    nlohmann::json raw = readRawJson(path);
    CHECK(raw["v_sync"].is_boolean());
    CHECK(raw["v_sync"].get<bool>() == true);
    CHECK(ConfigManagerBase::readConfig("v_sync") == "1");

    std::remove(path.c_str());
}

TEST_CASE("writeConfig int overload stores a JSON integer", "[ConfigManager]")
{
    const std::string path = "og_config_write.json";
    ConfigManagerBase::loadConfigFile(path);

    ConfigManagerBase::writeConfig("bots", 42);

    nlohmann::json raw = readRawJson(path);
    CHECK(raw["bots"].is_number_integer());
    CHECK(raw["bots"].get<int>() == 42);
    CHECK(ConfigManagerBase::readConfig("bots") == "42");

    std::remove(path.c_str());
}

TEST_CASE("writeConfig double overload stores a JSON float", "[ConfigManager]")
{
    const std::string path = "og_config_write.json";
    ConfigManagerBase::loadConfigFile(path);

    ConfigManagerBase::writeConfig("bot_speed_mult", 1.5);

    nlohmann::json raw = readRawJson(path);
    CHECK(raw["bot_speed_mult"].is_number_float());
    CHECK(raw["bot_speed_mult"].get<double>() == Catch::Approx(1.5));

    std::remove(path.c_str());
}

TEST_CASE("writeConfig string overload infers the JSON type from the value", "[ConfigManager]")
{
    const std::string path = "og_config_write.json";
    ConfigManagerBase::loadConfigFile(path);

    // Pass std::string explicitly: a string literal would match the bool
    // overload via the implicit const char* -> bool conversion.
    ConfigManagerBase::writeConfig("flag", std::string("true"));
    ConfigManagerBase::writeConfig("count", std::string("10"));
    ConfigManagerBase::writeConfig("ratio", std::string("1.25"));
    ConfigManagerBase::writeConfig("name", std::string("player_one"));

    nlohmann::json raw = readRawJson(path);

    CHECK(raw["flag"].is_boolean());
    CHECK(raw["flag"].get<bool>() == true);

    CHECK(raw["count"].is_number_integer());
    CHECK(raw["count"].get<int>() == 10);

    CHECK(raw["ratio"].is_number_float());
    CHECK(raw["ratio"].get<double>() == Catch::Approx(1.25));

    CHECK(raw["name"].is_string());
    CHECK(raw["name"].get<std::string>() == "player_one");

    std::remove(path.c_str());
}

TEST_CASE("readCommonConfigs and writeCommonConfigs round-trip", "[ConfigManager]")
{
    const std::string path = "og_config_common.json";

    ConfigManagerBase::level = "level_desert.json";
    ConfigManagerBase::bots = 5;
    ConfigManagerBase::bot_speed_mult = 0.25f;
    ConfigManagerBase::max_bullets = 15;
    ConfigManagerBase::fps_limit_server = 90;
    ConfigManagerBase::server_port = 7000;

    ConfigManagerBase::loadConfigFile(path);
    ConfigManagerBase::writeCommonConfigs();

    nlohmann::json raw = readRawJson(path);
    CHECK(raw["level"].get<std::string>() == "level_desert.json");
    CHECK(raw["bots"].is_number_integer());
    CHECK(raw["bots"].get<int>() == 5);
    CHECK(raw["bot_speed_mult"].is_number_float());
    CHECK(raw["max_bullets"].get<int>() == 15);
    CHECK(raw["fps_limit_server"].get<int>() == 90);
    CHECK(raw["server_port"].get<int>() == 7000);

    // Load back and confirm the typed settings are repopulated.
    ConfigManagerBase::level = "";
    ConfigManagerBase::bots = 0;
    ConfigManagerBase::bot_speed_mult = 0.0f;
    ConfigManagerBase::max_bullets = 0;
    ConfigManagerBase::fps_limit_server = 0;
    ConfigManagerBase::server_port = 0;

    ConfigManagerBase::loadConfigFile(path);
    ConfigManagerBase::readCommonConfigs();

    CHECK(ConfigManagerBase::level == "level_desert.json");
    CHECK(ConfigManagerBase::bots == 5);
    CHECK(ConfigManagerBase::bot_speed_mult == Catch::Approx(0.25));
    CHECK(ConfigManagerBase::max_bullets == 15);
    CHECK(ConfigManagerBase::fps_limit_server == 90);
    CHECK(ConfigManagerBase::server_port == 7000);

    std::remove(path.c_str());
}
