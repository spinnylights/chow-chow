#include <doctest.h>

#include "chow-chow/operator.hpp"

using namespace ChowChow;

TEST_CASE("one cycle") {
    const unsigned int sr = 48000;
    const unsigned int hz = 12000;

    Operator o {sr};
    o.frequency(hz);

    const auto start = o.position();

    o.advance(sr/hz);

    CHECK(o.position() == start);
    //CHECK(o.calc() == 0);
}
