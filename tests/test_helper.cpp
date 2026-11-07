#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>

#include <glm/glm.hpp>
#include <string>
#include <vector>

#include "Helper.h"

TEST_CASE("Helper::split splits on a delimiter", "[Helper]")
{
    std::vector<std::string> parts;

    SECTION("three tokens")
    {
        size_t n = Helper::split("a;b;c", parts, ';');
        REQUIRE(n == 3);
        REQUIRE(parts[0] == "a");
        REQUIRE(parts[1] == "b");
        REQUIRE(parts[2] == "c");
    }

    SECTION("single token (no delimiter)")
    {
        size_t n = Helper::split("hello", parts, ';');
        REQUIRE(n == 1);
        REQUIRE(parts[0] == "hello");
    }

    SECTION("empty-ish token at the end")
    {
        Helper::split("a;", parts, ';');
        REQUIRE(parts[0] == "a");
        REQUIRE(parts.size() == 2);
    }
}

TEST_CASE("Helper::trim removes surrounding whitespace", "[Helper]")
{
    std::string s;

    s = "  hello  ";
    REQUIRE(Helper::trim(s) == "hello");

    s = "\t\r\n value ";
    REQUIRE(Helper::trim(s) == "value");

    s = "   ";
    REQUIRE(Helper::trim(s) == "");

    s = "no trim";
    REQUIRE(Helper::trim(s) == "no trim");
}

TEST_CASE("Helper::eraseSubStr removes the first occurrence", "[Helper]")
{
    std::string s = "abcXdefXghi";
    Helper::eraseSubStr(s, "X");
    REQUIRE(s == "abcdefXghi");

    s = "no match here";
    Helper::eraseSubStr(s, "Z");
    REQUIRE(s == "no match here");
}

TEST_CASE("Helper::to_string_with_precision formats fixed precision", "[Helper]")
{
    REQUIRE(Helper::to_string_with_precision(1.23456789, 2) == "1.23");
    REQUIRE(Helper::to_string_with_precision(1.5, 1) == "1.5");
    REQUIRE(Helper::to_string_with_precision(10, 0) == "10");
}

TEST_CASE("glm vector string round-trips", "[Helper]")
{
    glm::vec2 v2{1.2f, -3.4f};
    glm::vec3 v3{1.2f, -3.4f, 5.6f};
    glm::vec4 v4{1.2f, -3.4f, 5.6f, 7.8f};

    glm::vec2 back2 = Helper::string_to_glmVec2(Helper::glmVec2_to_string(v2));
    glm::vec3 back3 = Helper::string_to_glmVec3(Helper::glmVec3_to_string(v3));
    glm::vec4 back4 = Helper::string_to_glmVec4(Helper::glmVec4_to_string(v4));

    REQUIRE(back2.x == Catch::Approx(v2.x).epsilon(1e-3f));
    REQUIRE(back2.y == Catch::Approx(v2.y).epsilon(1e-3f));

    REQUIRE(back3.x == Catch::Approx(v3.x).epsilon(1e-3f));
    REQUIRE(back3.y == Catch::Approx(v3.y).epsilon(1e-3f));
    REQUIRE(back3.z == Catch::Approx(v3.z).epsilon(1e-3f));

    REQUIRE(back4.x == Catch::Approx(v4.x).epsilon(1e-3f));
    REQUIRE(back4.w == Catch::Approx(v4.w).epsilon(1e-3f));
}

TEST_CASE("Helper::glmVec3_to_string produces the delimiter form", "[Helper]")
{
    REQUIRE(Helper::glmVec3_to_string(glm::vec3(1, 2, 3)) == "1.000000;2.000000;3.000000");
}

TEST_CASE("Helper::networkIDToString zero-pads to three digits", "[Helper]")
{
    REQUIRE(Helper::networkIDToString(0) == "000");
    REQUIRE(Helper::networkIDToString(5) == "005");
    REQUIRE(Helper::networkIDToString(50) == "050");
    REQUIRE(Helper::networkIDToString(123) == "123");
    REQUIRE(Helper::networkIDToString(1234) == "1234");
}
