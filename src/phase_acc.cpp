#include <iostream>

#include "chow-chow/phase_acc.hpp"

using P = ChowChow::PhaseAcc;

static constexpr int NDX_BITS = 11;
static constexpr uint_fast8_t FRAC_BITS = 64 - NDX_BITS;

void P::phase_incr(Frequency fr, uint64_t sr)
{
    f = fr;
    sample_r = sr;
    phase_inc = (fr * (TAU / sr)).raw();

    bottom_bit = phase_inc & 1;
}

P::PhaseAcc(Frequency fr, uint64_t sr)
{
    phase_incr(fr, sr);
}

void P::frequency(Frequency fr)
{
    f = fr;
    phase_incr(fr, sample_r);
}

void P::sample_rate(uint64_t sr)
{
    sample_r = sr;
    phase_incr(f, sr);
}

double ChowChow::amp_sin(uint64_t phase)
{
    static constexpr double TAU_U = static_cast<double>(UINT64_MAX) - 1.;
    static constexpr double TAU_F = 2*M_PI;
    return std::sin((static_cast<double>(phase) / TAU_U) * TAU_F);
}

constexpr uint64_t max_frac(uint64_t frac_bits)
{
    return ((uint64_t)1 << frac_bits) - 1;
}

double ChowChow::amp_linear(uint64_t phase)
{
    static constexpr uint64_t MAX_FRAC = max_frac(FRAC_BITS);

    uint64_t y0ndx = phase >> FRAC_BITS;
    double fract = static_cast<double>(phase & MAX_FRAC)/MAX_FRAC;

    uint64_t y1ndx = y0ndx + 1;
    if (y1ndx > SINE_TAB_LAST) y1ndx = 0;

    const auto y0  = SINE_TAB[y0ndx];
    const auto y1  = SINE_TAB[y1ndx];

    return (y0 + (y1 - y0)*fract);
}

double ChowChow::amp_direct(uint64_t phase)
{
    return SINE_TAB[phase >> FRAC_BITS];
}

double ChowChow::amp_circ(uint64_t phase)
{
    static constexpr int SMALL_NDX_BITS = 6;
    static constexpr int FRAC_BITS_P_SMALL_NDX_BITS = FRAC_BITS;
    static constexpr int FRAC_BITS = FRAC_BITS_P_SMALL_NDX_BITS - SMALL_NDX_BITS;
    static constexpr uint64_t MAX_FRAC = max_frac(FRAC_BITS);
    static constexpr double ONED_MAX_FRAC = 1./MAX_FRAC;

    const uint64_t sinndx = phase >> FRAC_BITS_P_SMALL_NDX_BITS;
    const uint64_t cosndx = (sinndx + 512) & 2047;

    const auto sinn = SINE_TAB[sinndx];
    const auto cosn = SINE_TAB[cosndx];

    const uint64_t small_ndx = (phase >> FRAC_BITS) & SMALL_TAB_LAST;

    const double fracsin_0 = SINE_SMALL_TAB[small_ndx];
    const double fraccos_0 = COS_SMALL_TAB[small_ndx];

    const uint64_t small_ndx_1 = small_ndx + 1;

    double fracsin_1 = 0.00306795676296597627;
    double fraccos_1 = 0.999995293809576172;
    if (small_ndx_1 <= SMALL_TAB_LAST) {
        fracsin_1 = SINE_SMALL_TAB[small_ndx_1];
        fraccos_1 = COS_SMALL_TAB[small_ndx_1];
    }

    const double frac = static_cast<double>(phase & MAX_FRAC) * ONED_MAX_FRAC;
    const double fracsin = fracsin_0 + (fracsin_1 - fracsin_0)*frac;
    const double fraccos = fraccos_0 + (fraccos_1 - fraccos_0)*frac;

    return (sinn*fraccos + cosn*fracsin);
}

double P::amp(uint64_t phase) const
{
    return (*amp_fn)(phase) * out_amp;
}

double P::amp() const
{
    return amp(ph);
}

union double_and_bits {
    double d;
    uint64_t u;
};

constexpr uint64_t double_to_bits(double x)
{
    return (double_and_bits){.d = x}.u;
}

constexpr double bits_to_double(uint64_t x)
{
    return (double_and_bits){.u = x}.d;
}

constexpr double pow2_fmod4(double x)
{
    uint64_t b = double_to_bits(x);

    constexpr uint_fast16_t COEFF_BITS = 52;
    constexpr uint_fast16_t EXPON_MASK = 0x7ff;
    uint_fast16_t e = b >> COEFF_BITS & EXPON_MASK;

    constexpr uint_fast16_t EXP_4_M1 = 1024;
    e = COEFF_BITS - (e - EXP_4_M1);

    b = b >> e << e;

    return x - bits_to_double(b);
}

double P::amp(double mod) const
{
    if (mod > 4.0 || mod < -4.0) {
        mod = pow2_fmod4(mod);
    }

    const uint64_t scaled_mod = static_cast<uint64_t>(mod * PI_2_SIGNED);

    const uint64_t signed_scale_ph = (ph >> 1) + scaled_mod;

    const uint64_t adj_ph = (signed_scale_ph << 1) + bottom_bit;

    return amp(adj_ph);
}

void P::advance() { ph += phase_inc; }

void P::advance(const PhaseAcc& vibr)
{
    advance();
    ph += static_cast<int64_t>(vibr.amp() * PI_2_SIGNED / sample_r);
}

void P::quality(uint_fast8_t q)
{
    if (q >= 4) {
        amp_fn = *amp_sin;
    } else if (q == 3) {
        amp_fn = *amp_circ;
    } else if (q == 2) {
        amp_fn = *amp_linear;
    } else {
        amp_fn = *amp_direct;
    }
}
