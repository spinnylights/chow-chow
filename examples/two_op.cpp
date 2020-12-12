#include <algorithm>
#include <iostream>

#include "chow-chow/operators.hpp"
#include "chow-chow/wav_file.hpp"

using namespace ChowChow;

int main(void)
{
    constexpr size_t SAMPLE_RATE = 48000;
    constexpr size_t LENGTH_SECS = 8;
    constexpr size_t LENGTH = SAMPLE_RATE * LENGTH_SECS;

    Operators<2> ops;

    ops.sample_rate(SAMPLE_RATE);

    static constexpr double FREQ = 250.;
    ops[2].freq(FREQ);
    ops[1].freq(FREQ);

    ops[2].ratio(3.35);

    ops.connect(2, 1, 1.8);

    ops.output(1);

    ops.reorder();

    const std::vector<double> ramp = [&]{
        std::vector<double> e;

        for (size_t i = 0; i < LENGTH; ++i) {
            const double x = static_cast<double>(i) / LENGTH;
            e.push_back(1.0 - x);
        }

        return e;
    }();

    const std::vector<double> env = [&]{
        std::vector<double> e;

        const size_t full_vol = LENGTH*4/5;
        for (size_t i = 0; i < full_vol; ++i) {
            e.push_back(1.0);
        }

        const size_t decay = LENGTH - full_vol;
        for (size_t i = 0; i < decay; ++i) {
            const double x = static_cast<double>(i) / (decay);
            e.push_back(1.0 - x);
        }

        return e;
    }();

    std::vector<double> out;

    for (std::size_t i = 0; i < LENGTH; ++i) {
        ops[2].index(0.25 + ramp[i]);

        const auto sig = ops.sig() * env[i];

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
