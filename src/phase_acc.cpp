#include "chow-chow/phase_acc.hpp"

using P = ChowChow::PhaseAcc;

void P::phase_incr(Frequency fr, phase_t sr)
{
    f = fr;
    sample_r = sr;
    phase_inc = (fr * (TAU / sr)).raw();
}

P::PhaseAcc(Frequency fr, phase_t sr)
{
    phase_incr(fr, sr);
}

void P::frequency(Frequency fr)
{
    f = fr;
    phase_incr(fr, sample_r);
}

void P::sample_rate(phase_t sr)
{
    sample_r = sr;
    phase_incr(f, sr);
}

P::amp_t P::amp(phase_t phase) const
{
    static constexpr int NDX_BITS = 11;
    static constexpr int SMALL_NDX_BITS = 6;
    static constexpr int FRAC_BITS = 64 - NDX_BITS - SMALL_NDX_BITS;
    static constexpr int FRAC_BITS_P_SMALL_NDX_BITS = FRAC_BITS + SMALL_NDX_BITS;
    static constexpr uint64_t MAX_FRAC = 140737488355327;
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

    return (sinn*fraccos + cosn*fracsin) * out_amp;
}

P::amp_t P::amp() const
{
    return amp(ph);
}

P::amp_t P::amp(double mod) const
{
    const uint_fast8_t bottom_bit = ph & 1;
    phase_t signed_scale_ph = ph >> 1;

    signed_scale_ph += static_cast<phase_t_signed>(mod*PI_2_SIGNED);

    const phase_t adj_ph = (signed_scale_ph << 1) + bottom_bit;

    return amp(adj_ph);
}

void P::advance() { ph += phase_inc; }

void P::advance(const PhaseAcc& vibr)
{
    advance();
    ph += static_cast<phase_t_signed>(vibr.amp() * PI_2_SIGNED / sample_r);
}
