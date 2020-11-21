#include <vector>
#include <cmath>
#include <numeric>
#include <algorithm>

#include "chow-chow/operators.hpp"
#include "chow-chow/constants.hpp"
#include "chow-chow/wav_file.hpp"

using namespace ChowChow;

int main(void)
{
    const double freq = 73.42; // ~d2

    /*      |-|
     *     [4]-
     *  |---|---|
     * [1] [2] [3]
     */

    const double vib_frq = 10;
    const double vib_amp = 1.;

    Operators<4> ops;

    ops[4].freq(freq);
    ops[4].freq_offset(-.912);
    ops[4].vibrato_freq(vib_frq);
    ops[4].vibrato_amp(vib_amp);

    ops[3].freq(freq);
    ops[3].freq_offset(-.801);
    ops[3].ratio(2.);
    ops[3].vibrato_freq(vib_frq * 1.002);
    ops[3].vibrato_amp(vib_amp);

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

    ops.connect(4, 4);
    ops.connect(4, 3);
    ops.connect(4, 2);
    ops.connect(4, 1);
    ops.reorder();

    ops.output(3);
    ops.output(2);
    ops.output(1);

    constexpr size_t SAMPLE_RATE = 48000;
    constexpr size_t OVERSAMPLING = 4;
    constexpr size_t LENGTH_SECS = 20;

    constexpr size_t RATE = SAMPLE_RATE * OVERSAMPLING;
    constexpr size_t LENGTH = SAMPLE_RATE * LENGTH_SECS;
    constexpr size_t OVERSAMP_LENGTH = LENGTH * OVERSAMPLING;

    const std::vector<double> envelope = [&]{
        std::vector<double> e;

        const size_t len = OVERSAMP_LENGTH;
        const size_t frac = 275;
        const size_t up = len/frac;
        const size_t down = len - up;

        for (size_t i = 0; i < up; ++i) {
            const double x = static_cast<double>(i) / up;
            e.push_back(x);
        }

        for (size_t i = 0; i < down; ++i) {
            const double shape = 2.;
            const double x = static_cast<double>(i*shape) / down;
            const double point =
                1.0
                + (-1.0
                   * (1.0 - std::exp(x))
                   / (1.0 - std::exp(shape)));
            e.push_back(point);
        }

        return e;
    }();

    const std::vector<double> ramp = [&]{
        std::vector<double> e;

        const size_t len = OVERSAMP_LENGTH;
        const size_t frac = 275;
        const size_t up = len/frac;
        const size_t down = len - up;

        for (size_t i = 0; i < up; ++i) {
            const double x = static_cast<double>(i) / up;
            e.push_back(x);
        }

        for (size_t i = 0; i < down; ++i) {
            const double x = static_cast<double>(i) / down;
            e.push_back(std::exp(-1.2 * x));
        }

        return e;
    }();

    std::vector<long double> samples;

    std::array<double, OVERSAMPLING> oversample_win;

    for (size_t i = 0; i < OVERSAMP_LENGTH; (void)0) {
        for (size_t j = 0; j < OVERSAMPLING; ++j) {
            const auto sample_n = i + j;

            const double theta = static_cast<double>(sample_n) / RATE;

            ops[4].index(5.969 * ramp[i]);
            ops[3].index(8.231 * ramp[i]);
            ops[2].index(8.231 * ramp[i]);
            ops[1].index(20.96 * ramp[i]);

            ops.connect(4, 3, 1.87 * ramp[i]);
            ops.connect(4, 2, 1.87 * ramp[i]);
            ops.connect(4, 1, 1.87 * ramp[i]);

            oversample_win[j] = ops.sig(theta) * envelope[i];
        }
        i += OVERSAMPLING;

        double sample =
                  std::accumulate(oversample_win.begin(),
                                  oversample_win.end(),
                                  0.)
                / OVERSAMPLING;

        samples.push_back(sample);
    }

    constexpr size_t offset_1 = 0.02425 * SAMPLE_RATE;
    constexpr size_t offset_2 = 0.01577 * SAMPLE_RATE;
    constexpr size_t offset_3 = 0.0167 * SAMPLE_RATE;

    constexpr size_t LEN_TOTAL = LENGTH + offset_1;

    constexpr double l_chan_1_amp = 0.197;
    constexpr double l_chan_2_amp = 0.071;
    constexpr double r_chan_1_amp = 0.207;
    constexpr double r_chan_2_amp = 0.061;

    std::vector<double> l_chan_1 (LEN_TOTAL, 0.);
    for (size_t i = 0; i < samples.size(); ++i) {
        l_chan_1[i] = samples[i] * l_chan_1_amp;
    }

    std::vector<double> l_chan_2 (LEN_TOTAL, 0.);
    for (size_t i = 0; i < samples.size(); ++i) {
        l_chan_2[i + offset_2] = samples[i] * l_chan_2_amp;
    }

    std::vector<double> r_chan_1 (LEN_TOTAL, 0.);
    for (size_t i = 0; i < samples.size(); ++i) {
        r_chan_1[i + offset_1] = samples[i] * r_chan_1_amp;
    }

    std::vector<double> r_chan_2 (LEN_TOTAL, 0.);
    for (size_t i = 0; i < samples.size(); ++i) {
        r_chan_2[i + offset_3] = samples[i] * r_chan_2_amp;
    }

    std::vector<long double> out;
    for (size_t i = 0; i < LEN_TOTAL; ++i) {
        const auto l = l_chan_1[i] + l_chan_2[i];
        const auto r = r_chan_1[i] + r_chan_2[i];

        const double cross = 0.26;
        out.push_back(l + r*cross);
        out.push_back(r + l*cross);
    }

    const auto max = std::max_element(out.begin(), out.end());
    const double norm_div = 0.95 / *max;
    for (auto &x : out) {
        x *= norm_div;
    }

    ChowChow::WAVFile file {out, SAMPLE_RATE};
    file.write("./gunstar.wav");

    return 0;
}
