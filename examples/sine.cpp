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

    Operators<1> ops;

    ops.sample_rate(SAMPLE_RATE);

    ops[1].freq(440.);

    ops.output(1);

    std::vector<double> out;

    for (std::size_t i = 0; i < LENGTH; ++i) {
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
    file.write("./sine.wav");
}
