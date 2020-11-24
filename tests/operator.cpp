#include <doctest.h>

#include "chow-chow/operator.hpp"

using namespace ChowChow;

TEST_CASE("sig with mod") {
    Operator o;

    CHECK(o.sig() == 0.);
    CHECK(o.sig(1.0) == 1.0);
}

TEST_CASE("advancing") {
    Operator o;

    o.freq(16);
    o.sample_rate(64);

    o.advance();

    CHECK(o.sig() == 1.0);
}
