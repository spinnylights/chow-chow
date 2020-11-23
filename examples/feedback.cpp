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

    Operator op_1;
    Operator op_2;

    op_1.sample_rate(SAMPLE_RATE);
    op_2.sample_rate(SAMPLE_RATE);

    op_1.freq(200.);
    op_2.freq(200.);

    op_1.add_modulator(op_2);

    std::vector<double> out;

    for (std::size_t i = 0; i < LENGTH; ++i) {
        const auto sig = op_1.sig();

        out.push_back(sig);
        out.push_back(sig);

        op_2.advance();
        op_1.advance();
    }

    const auto max = std::max_element(out.begin(), out.end());
    const double norm_div = 0.95 / *max;
    for (auto &x : out) {
        x *= norm_div;
    }

    ChowChow::WAVFile file {out, SAMPLE_RATE};
    file.write("./sine.wav");
}
