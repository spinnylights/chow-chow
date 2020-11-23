#include <doctest.h>

#include <cmath>

#include <chow-chow/frequency.hpp>

using namespace ChowChow;

void comp_w_double(Frequency f, double expected)
{
    CHECK(std::abs(f.make_double() - expected) < 1.e-10);
}

TEST_CASE("init from double") {
    double expected = 128.5;
    Frequency f = expected;
    CHECK(f.intg() == 128);
    CHECK(f.frac() == Frequency::MAX_FRAC / 2);
    comp_w_double(f, expected);
}

TEST_CASE("init from int") {
    unsigned int expected = 440;
    Frequency f = expected;
    CHECK(f.intg() == expected);
    CHECK(f.frac() == 0);
}

TEST_CASE("adding") {
    Frequency f = 2.2;
    Frequency g = 4.4;
    comp_w_double(f + g, 6.6);
}

TEST_CASE("subtracting") {
    Frequency f = 4.4;
    Frequency g = 2.2;
    comp_w_double((f - g), 2.2);
}

TEST_CASE("multiplying") {
    Frequency f = 10;
    Frequency g = 2.2;
    comp_w_double((f * g), 22.);
}

TEST_CASE("dividing") {
    Frequency f = 2.2;
    Frequency g = 10;
    comp_w_double((f / g), .22);
}

TEST_CASE("equality") {
    Frequency f = 2.2;
    Frequency g = 2.2;
    CHECK(f == g);
}

TEST_CASE("inequality") {
    Frequency f = 2.2;
    Frequency g = 2.3;
    CHECK(f != g);
}
