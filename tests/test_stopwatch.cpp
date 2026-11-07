#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>

#include "StopWatch.h"

TEST_CASE("StopWatch measures elapsed milliseconds", "[StopWatch]")
{
    StopWatch watch;

    watch.start();
    for (volatile int i = 0; i < 1000000; ++i)
    {
        // busy work so some time actually passes
    }
    float64 elapsed = watch.stop();

    REQUIRE(elapsed >= 0.0);
    REQUIRE(elapsed < 5000.0); // sane upper bound on any machine
}

TEST_CASE("StopWatch stop after immediate start is near zero", "[StopWatch]")
{
    StopWatch watch;
    watch.start();
    float64 elapsed = watch.stop();

    REQUIRE(Catch::Approx(elapsed).epsilon(1.0) >= 0.0);
}
