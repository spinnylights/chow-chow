#include <vector>
#include <cmath>
#include <numeric>
#include <algorithm>

#include "chow-chow/wav_file.hpp"

static constexpr double TAU = 2*M_PI;
static constexpr double TAU_SCALE = 1./TAU;

struct Operator {
    // base frequency of osc.
    double freq;

    // for chorus effects
    double freq_offset;

    // coeff of freq
    // determines spread of partials in carriers
    double ratio;

    // lfo hz
    double vibrato_freq;

    // .0001 – .00001 or so
    double vibrato_amp;

    // # of partials in carriers
    // also amp of osc
    double index;

    double sig(double modulator, double phi) const
    {
        double vibrato = freq * vibrato_amp * std::sin(vibrato_freq * phi);
        double freq_base = freq + vibrato;
        double freq_rat = freq_base * ratio;
        double freq_fin = freq_rat + (freq_rat * freq_offset);

        return std::sin(freq_fin * phi + modulator * TAU) * index;
    }
};

int main(void)
{
    const double freq = 73.42; // ~d2

    /*      |-|
     *     [4]-
     *  |---|---|
     * [1] [2] [3]
     */

    const double vib_frq = 10;
    const double vib_amp = .0001;

    Operator op_4;
    op_4.freq = freq;
    op_4.freq_offset = -.00912;
    op_4.ratio = 1.;
    op_4.vibrato_freq = vib_frq;
    op_4.vibrato_amp = vib_amp;
    op_4.index = 1.95;

    Operator op_3;
    op_3.freq = freq;
    op_3.freq_offset = -.00801;
    op_3.ratio = 2.;
    op_3.vibrato_freq = vib_frq * 1.002;
    op_3.vibrato_amp = vib_amp;
    op_3.index = 1.31;

    Operator op_2;
    op_2.freq = freq;
    op_2.freq_offset = .00648;
    op_2.ratio = 0.5;
    op_2.vibrato_freq = vib_frq * 0.998;
    op_2.vibrato_amp = vib_amp;
    op_2.index = 1.31;

    Operator op_1;
    op_1.freq = freq;
    op_1.freq_offset = -.00763;
    op_1.ratio = 5.;
    op_1.vibrato_freq = vib_frq * (1. - (1. / 3.192));
    op_1.vibrato_amp = vib_amp * 0.512;
    op_1.index = 3.336;

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

            const double phi = sample_n * TAU / RATE;

            op_4.index = 0.95 * ramp[i];
            op_3.index = 1.31 * ramp[i];
            op_2.index = 1.31 * ramp[i];
            op_1.index = 3.336 * ramp[i];

            const auto sig_4_1 = op_4.sig(0., phi);
            const auto sig_4   = op_4.sig(sig_4_1, phi);
            const auto sig_3   = op_3.sig(sig_4 * 1.87 * ramp[i], phi);
            const auto sig_2   = op_2.sig(sig_4 * 1.87 * ramp[i], phi);
            const auto sig_1   = op_1.sig(sig_4 * 1.87 * ramp[i], phi);

            const auto sig = sig_1 + sig_2 + sig_3;
            oversample_win[j] = sig * envelope[i];
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
