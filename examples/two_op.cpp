#include <algorithm>
#include <iostream>

#include "chow-chow/operators.hpp"
#include "chow-chow/wav_file.hpp"

using namespace ChowChow;

int main(void)
{
    constexpr size_t SAMPLE_RATE = 48000;
    constexpr size_t LENGTH_SECS = 4;
    constexpr size_t LENGTH = SAMPLE_RATE * LENGTH_SECS;

    Operators<2> ops;

    ops.sample_rate(SAMPLE_RATE);

    ops[2].freq(100.);
    ops[1].freq(100.);

    ops[2].ratio(2.0);

    ops.connect(2, 1);
    //ops.connect(1, 2);

    ops.output(1);

    const std::vector<double> ramp = [&]{
        std::vector<double> e;

        const size_t len = LENGTH;
        const size_t frac = 275;
        const size_t up = len/frac;
        const size_t down = len;

        for (size_t i = 0; i < len; ++i) {
            const double x = static_cast<double>(i) / len;
            e.push_back(std::exp(-5.2 * x));
        }

        return e;
    }();

    std::vector<double> out;

    for (std::size_t i = 0; i < LENGTH; ++i) {
        ops[2].index(1.0*ramp[i]);

        const auto sig = ops.sig();

        out.push_back(sig);
        out.push_back(sig);

        ops.advance();
    }

    const auto max = std::max_element(out.begin(), out.end());
    const double norm_div = 0.95 / *max;
    for (auto &x : out) {
        x *= norm_div;
    }

    ChowChow::WAVFile file {out, SAMPLE_RATE};
    file.write("./two_op.wav");
}
