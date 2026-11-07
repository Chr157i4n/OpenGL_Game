#include <catch2/catch_test_macros.hpp>

#include <iostream>
#include <sstream>
#include <string>
#include <regex>

#include "Logger.h"

TEST_CASE("Logger::log writes a [hh:mm:ss] prefix before the message", "[Logger]")
{
    std::ostringstream buffer;
    std::streambuf* old = std::cout.rdbuf(buffer.rdbuf());

    Logger::log("hello world");

    std::cout.rdbuf(old);

    std::string output = buffer.str();
    std::regex pattern(R"(\d{2}:\d{2}:\d{2}\] hello world\s*$)");

    REQUIRE(output.find("[") == 0);
    REQUIRE(std::regex_search(output, pattern));
}

TEST_CASE("Logger::log handles an empty message", "[Logger]")
{
    std::ostringstream buffer;
    std::streambuf* old = std::cout.rdbuf(buffer.rdbuf());

    Logger::log("");

    std::cout.rdbuf(old);

    REQUIRE(buffer.str().find("] ") != std::string::npos);
}
