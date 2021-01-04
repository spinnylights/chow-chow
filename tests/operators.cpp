#include <doctest.h>

#include <vector>

#include "chow-chow/operators.hpp"

using namespace ChowChow;

TEST_CASE("setting output") {
    Operators<2> ops;

    ops.output(1);

    CHECK(ops.is_output(1));
}

void simple_sig_test(Operators<2>& ops, double initial_epsilon)
{
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
    ops.output(1);

    CHECK(std::fabs(ops.sig()) < initial_epsilon);
    CHECK(std::abs(ops.sig()) > 1e-10);

    CHECK(ops[2].sig() == ops.check_sig(2));
    CHECK(ops[1].sig() == ops.check_sig(1));
}

TEST_CASE("simple sig") {
    Operators<2> ops;

    simple_sig_test(ops, 1e-15);
}

TEST_CASE("oversampling") {
    Operators<2> ops;
    ops.oversampling(4);

    simple_sig_test(ops, 1e-1);
}
