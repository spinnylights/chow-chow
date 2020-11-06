#include <doctest.h>

#include "chow-chow/sine.hpp"

using namespace ChowChow;

TEST_CASE("sine table") {
    SUBCASE("0") {
        CHECK(SINE_TAB[0] == 0);
    }

    SUBCASE("1 past 0") {
        CHECK(SINE_TAB[1] == 6);
    }

    SUBCASE("2 past 0") {
        CHECK(SINE_TAB[2] == 13);
    }

    SUBCASE("1/16") {
        CHECK(SINE_TAB[SINE_TAB_LEN * 0.0625] == 12539);
    }

    SUBCASE("1/8") {
        CHECK(SINE_TAB[SINE_TAB_LEN * 0.125] == 23170);
    }

    SUBCASE("1/4") {
        CHECK(SINE_TAB[SINE_TAB_LEN * 0.25] == 32767);
    }

    SUBCASE("1/2") {
        CHECK(SINE_TAB[SINE_TAB_LEN * 0.5] == 0);
    }

    SUBCASE("9/16") {
        CHECK(SINE_TAB[SINE_TAB_LEN * 0.5625] == -12540);
    }

    SUBCASE("5/8") {
        CHECK(SINE_TAB[SINE_TAB_LEN * 0.625] == -23170);
    }

    SUBCASE("3/4") {
        CHECK(SINE_TAB[SINE_TAB_LEN * 0.75] == -32768);
    }

    SUBCASE("2 before 0") {
        CHECK(SINE_TAB[SINE_TAB_LEN - 2] == -13);
    }

    SUBCASE("1 before 0") {
        CHECK(SINE_TAB[SINE_TAB_LEN - 1] == -6);
    }
}
