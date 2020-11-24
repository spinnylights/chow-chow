#include <algorithm>
#include <iostream>
#include <cmath>

#include "chow-chow/operators.hpp"
#include "chow-chow/wav_file.hpp"

using namespace ChowChow;

int main(void)
{
    constexpr size_t SAMPLE_RATE = 48000;
    constexpr size_t LENGTH_SECS = 8;
    constexpr size_t LENGTH = SAMPLE_RATE * LENGTH_SECS;

    Operators<1> ops;

    ops[1].freq(200.);

    ops.connect(1, 1, 1.);

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

        for (size_t i = 0; i < LENGTH*4/5; ++i) {
            e.push_back(1.0);
        }

        for (size_t i = 0; i < LENGTH/5; ++i) {
            const double x = static_cast<double>(i) / (LENGTH/5);
            e.push_back(1.0 - x);
        }

        return e;
    }();

    std::vector<double> out;

    for (std::size_t i = 0; i < LENGTH; ++i) {
        ops.connect(1, 1, ramp[i]);

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
    file.write("./feedback.wav");
}
