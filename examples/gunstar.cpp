#include <vector>
#include <cmath>
#include <numeric>
#include <algorithm>
#include <array>

#include "chow-chow/operators.hpp"
#include "chow-chow/constants.hpp"
#include "chow-chow/wav_file.hpp"

using namespace ChowChow;

static constexpr std::size_t SAMPLE_RATE = 48000;
static constexpr std::size_t OVERSAMPLING = 4;
static constexpr std::size_t LENGTH_SECS = 20;

static constexpr std::size_t RATE = SAMPLE_RATE * OVERSAMPLING;
static constexpr std::size_t LENGTH = SAMPLE_RATE * LENGTH_SECS;
static constexpr std::size_t OVERSAMP_LENGTH = LENGTH * OVERSAMPLING;

static constexpr double FREQ = 73.42; // ~d2

static constexpr double VIB_FRQ = 40.0;
static constexpr double VIB_AMP = 15.25;

#ifdef _WIN32
static constexpr double M_PI = 3.14159265358979323846;
#endif

void set_up_ops(Operators<4>& ops)
{
    /*      |-|
     *     [4]-
     *  |---|---|
     * [1] [2] [3]
     */

    ops.sample_rate(RATE);

    ops[4].freq(FREQ);
    ops[4].ratio(1.0);
    ops[4].freq_offset(-.912);
    ops[4].vibrato_freq(VIB_FRQ);
    ops[4].vibrato_amp(VIB_AMP);

    ops[3].freq(FREQ);
    ops[3].freq_offset(-.801);
    ops[3].ratio(2.0);
    ops[3].vibrato_freq(VIB_FRQ * 0.882);
    ops[3].vibrato_amp(VIB_AMP * 0.5);

    ops[2].freq(FREQ);
    ops[2].freq_offset(.648);
    ops[2].ratio(0.5);
    ops[2].vibrato_freq(VIB_FRQ * 0.998);
    ops[2].vibrato_amp(VIB_AMP);

    ops[1].freq(FREQ);
    ops[1].freq_offset(-.763);
    ops[1].ratio(5.);
    ops[1].vibrato_freq(VIB_FRQ * (1. - (1. / 3.192)));
    ops[1].vibrato_amp(VIB_AMP * 0.512);

    ops.connect(4, 4);
    ops.connect(4, 3);
    ops.connect(4, 2);
    ops.connect(4, 1);

    ops.output(3);
    ops.output(2);
    ops.output(1);

    ops.reorder();
}

void make_envelope(std::vector<double>& e)
{
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
}

void make_ramp(std::vector<double>& r)
{
    const size_t len = OVERSAMP_LENGTH;
    const size_t frac = 375;
    const size_t up = len/frac;
    const size_t down = len - up;

    for (size_t i = 0; i < up; ++i) {
        const double x = static_cast<double>(i) / up;
        r.push_back(x);
    }

    for (size_t i = 0; i < down; ++i) {
        const double x = static_cast<double>(i) / down;
        r.push_back(std::exp(-1.2 * x));
    }
}

void run_synth(std::vector<double>& samples)
{
    Operators<4> ops;
    set_up_ops(ops);

    std::vector<double> envelope;
    make_envelope(envelope);

    std::vector<double> ramp;
    make_ramp(ramp);

    std::array<double, OVERSAMPLING> oversample_win;

    for (size_t i = 0; i < OVERSAMP_LENGTH; (void)0) {
        for (size_t j = 0; j < OVERSAMPLING; ++j) {
            static const double scale_fact = .7;

            const double scale_fact_ramped = scale_fact * ramp[i];
            ops[4].index(5.969 * scale_fact_ramped);
            ops[3].index(8.231 * scale_fact_ramped);
            ops[2].index(8.231 * scale_fact_ramped);
            ops[1].index(20.96 * scale_fact_ramped);

            const double connection_str_ramped = 1.87 * scale_fact_ramped;
            ops.connect(4, 3, connection_str_ramped);
            ops.connect(4, 2, connection_str_ramped);
            ops.connect(4, 1, connection_str_ramped);

            const double vib_amp_ramped = VIB_AMP - VIB_AMP * ramp[i];
            ops[4].vibrato_amp(vib_amp_ramped);
            ops[3].vibrato_amp(vib_amp_ramped);
            ops[2].vibrato_amp(vib_amp_ramped);
            ops[1].vibrato_amp(vib_amp_ramped);

            const double vib_amp_ramped_prdic = vib_amp_ramped * M_PI;
            ops[4].vibrato_freq(std::sin(vib_amp_ramped_prdic * 239) * 40 + VIB_FRQ);
            ops[3].vibrato_freq(std::sin(vib_amp_ramped_prdic * 182) * 38 + VIB_FRQ);
            ops[2].vibrato_freq(std::sin(vib_amp_ramped_prdic * 403) * 18 + VIB_FRQ);
            ops[1].vibrato_freq(std::sin(vib_amp_ramped_prdic * 213) * 90 + VIB_FRQ);

            oversample_win[j] = ops.sig() * envelope[i];

            ops.advance();
        }
        i += OVERSAMPLING;

        double sample =
                  std::accumulate(oversample_win.begin(),
                                  oversample_win.end(),
                                  0.)
                / OVERSAMPLING;

        samples.push_back(sample);
    }
}

class Channel {
public:

    Channel()
    {}

    Channel(std::size_t sz)
    {
        samps.resize(sz, 0.);
    }

    void write(const std::vector<double>& samples,
               size_t offset,
               double amp)
    {
        for (size_t i = 0; i < samples.size(); ++i) {
            samps[i + offset] = samples[i] * amp;
        }
    }

    const double at(std::size_t i) const
    {
        return samps.at(i);
    }

private:

    std::vector<double> samps;

};

template <size_t N>
class Offsets {
public:

    Offsets()
        : os{}
    {}

    Offsets(const std::array<double, N>& os_prescale)
    {
        for (size_t i = 0; i < N; ++i) {
            os[i] = static_cast<size_t>(os_prescale[i] * SAMPLE_RATE);
        }
    }

    size_t max()
    {
        return *std::max_element(os.begin(), os.end());
    }

    const double at(std::size_t i) const
    {
        return os.at(i);
    }

private:

    std::array<size_t, N> os;
};

template <size_t N>
class MonoChannels {
public:

    MonoChannels(std::size_t chnl_len)
    {
        for (auto& c : chnls) {
            c = Channel(chnl_len);
        }

        std::fill(as.begin(), as.end(), 1.);
    }

    void offsets(Offsets<N> offsets) { ofs = offsets; }

    void amps(std::array<double, N> a) { as = a; }

    void write(const std::vector<double>& samples)
    {
        for (std::size_t i = 0; i < N; ++i) {
            chnls[i].write(samples, ofs.at(i), as[i]);
        }
    }

    const double sample(std::size_t i)
    {
        double s = 0.;

        for (const auto& c : chnls) {
            s += c.at(i);
        }

        return s;
    }

private:

    std::array<Channel, N> chnls;
    Offsets<N> ofs = {};
    std::array<double, N> as;
};

template <size_t N>
class StereoChannels {
public:

    StereoChannels(Offsets<N> l_ofs, Offsets<N> r_ofs)
        : len{LENGTH + std::max(l_ofs.max(), r_ofs.max())},
          left{len},
          right{len}
    {
        left.offsets(l_ofs);
        right.offsets(r_ofs);
    }

    void amps(std::array<double, N> l_a, std::array<double, N> r_a)
    {
        left.amps(l_a);
        right.amps(r_a);
    }

    void write_to(std::vector<double>& out,
                  const std::vector<double>& samps,
                  double cross_amp)
    {
        left.write(samps);
        right.write(samps);

        for (std::size_t i = 0; i < len; ++i) {
            const auto l = left.sample(i);
            const auto r = right.sample(i);

            out.push_back(l + r*cross_amp);
            out.push_back(r + l*cross_amp);
        }
    }

private:

    size_t len;
    MonoChannels<N> left;
    MonoChannels<N> right;
};

void stereo_spread(std::vector<double>& out,
                   const std::vector<double>& samples)
{
    static constexpr size_t CHAN_CNT = 4;

    StereoChannels<CHAN_CNT/2> chans = { {{ 0, 0.01577 }},
                                         {{ 0.02425, 0.0167 }} };

    chans.amps({ 0.197, 0.071 }, { 0.207, 0.061 });

    chans.write_to(out, samples, 0.26);
}

void normalize(std::vector<double>& out)
{
    const auto max = std::max_element(out.begin(), out.end());
    const double norm_div = 0.95 / *max;
    for (auto &x : out) {
        x *= norm_div;
    }
}

int main(void)
{
    std::vector<double> samples;
    run_synth(samples);

    std::vector<double> out;
    stereo_spread(out, samples);
    normalize(out);

    ChowChow::WAVFile file {out, SAMPLE_RATE};
    file.write("./gunstar.wav");

    return 0;
}
