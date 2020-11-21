#include <doctest.h>

#include <vector>

#include "chow-chow/operators.hpp"

using namespace ChowChow;

TEST_CASE("changing connection") {
    Operators<2> ops;

    CHECK(ops.connection(2, 1) == 0.);

    ops.connect(2, 1);

    ops.reorder();

    CHECK(ops.connection(2, 1) == 1.);
}

TEST_CASE("linear") {
    Operators<3> ops;

    ops.connect(3, 2);
    ops.connect(2, 1);

    ops.reorder();

    const std::vector<size_t> exp = {2, 1, 0};
    const auto actual = ops.order();

    CHECK(exp[0] == actual.at(0));
    CHECK(exp[1] == actual.at(1));
    CHECK(exp[2] == actual.at(2));
}

TEST_CASE("simple self-modulation") {
    Operators<2> ops;

    ops.connect(2, 2);

    ops.reorder();

    const std::vector<size_t> exp = {1, 1, 0};
    const auto actual = ops.order();

    CHECK(exp[0] == actual.at(0));
    CHECK(exp[1] == actual.at(1));
    CHECK(exp[2] == actual.at(2));
}

TEST_CASE("single feedback") {
    Operators<3> ops;

    ops.connect(3, 2);
    ops.connect(2, 1);
    ops.connect(1, 3);

    ops.reorder();

    const std::vector<size_t> exp = {2, 1, 0, 2, 1, 0};
    const auto actual = ops.order();

    for (std::size_t i = 0; i < exp.size(); ++i) {
        CHECK(exp[i] == actual.at(i));
    }
}

TEST_CASE("feedback and self-modulation") {
    Operators<3> ops;

    ops.connect(3, 2);
    ops.connect(2, 2);
    ops.connect(2, 1);
    ops.connect(1, 3);

    ops.reorder();

    const std::vector<size_t> exp = {2, 1, 1, 0, 2, 1, 1, 0};
    const auto actual = ops.order();

    for (std::size_t i = 0; i < exp.size(); ++i) {
        CHECK(exp[i] == actual.at(i));
    }
}

TEST_CASE("double feedback") {
    Operators<4> ops;

    ops.connect(4, 3);
    ops.connect(3, 2);
    ops.connect(2, 3);
    ops.connect(2, 1);
    ops.connect(1, 4);

    ops.reorder();

    const std::vector<size_t> exp =
        {3, 2, 1, 2, 1, 0, 3, 2, 1, 2, 1, 0 };
    const auto actual = ops.order();

    for (std::size_t i = 0; i < exp.size(); ++i) {
        CHECK(exp[i] == actual.at(i));
    }
}

TEST_CASE("setting output") {
    Operators<2> ops;

    ops.output(1);

    CHECK(ops.is_output(1));
}

TEST_CASE("simple sig") {
    Operators<2> ops;

    const double freq = 73.42; // ~d2
    const double vib_frq = 10;
    const double vib_amp = 1.;

    ops[2].freq(freq);
    ops[2].freq_offset(.648);
    ops[2].ratio(0.5);
    ops[2].vibrato_freq(vib_frq * 0.998);
    ops[2].vibrato_amp(vib_amp);

    ops[1].freq(freq);
    ops[1].freq_offset(-.763);
    ops[1].ratio(5.);
    ops[1].vibrato_freq(vib_frq * (1. - (1. / 3.192)));
    ops[1].vibrato_amp(vib_amp * 0.512);

    ops.connect(2, 1);
    ops.reorder();
    ops.output(1);

    const double time = 0.539;
    const auto sig = ops.sig(time);
    CHECK(sig == ops.check_sig(1));
    CHECK(ops[2].sig(time) == ops.check_sig(2));
    CHECK(ops[1].sig(time, ops[2].sig(time)) == ops.check_sig(1));
}
