#include <iostream>

#include "chow-chow/phase_acc.hpp"

using P = ChowChow::PhaseAcc;

static constexpr int NDX_BITS = 11;
static constexpr uint_fast8_t FRAC_BITS = 64 - NDX_BITS;

#ifdef _WIN32
static constexpr double M_PI   = 3.14159265358979323846;
static constexpr double M_PI_2 = 1.57079632679489661923;
#endif

union double_and_bits {
    double d;
    uint64_t l;
    int32_t i[2];
};

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

/* [-π/2, π/2] */
union double_and_bits pi_d2_scale(uint64_t phase)
{
    constexpr int64_t PI_D2_I = UINT64_MAX >> 2;
    constexpr double INV_PI_D2_I = 2.168404344971009e-19;

    int32_t sign = phase >> 63;
    int64_t side = (phase >> 62) & 1;
    int64_t r = phase & 0x3fffffffffffffff;
    r = std::labs((PI_D2_I*side - r));

    union double_and_bits y;
    y.d = static_cast<double>(r) * INV_PI_D2_I * M_PI_2;
    y.i[1] ^= sign << 31;

    return y;
}

/* Accurate in the worst case to within 18 ULPs or 2.0e-15. */
double ChowChow::amp_taylor(uint64_t phase)
{
    union double_and_bits y = pi_d2_scale(phase);

    /* coefficients from ARM's optimized sin routine at
     * https://github.com/ARM-software/optimized-routines/blob/master/math/v_sin.c;
     * massaged taylor series
     */
    constexpr double C7 = -7.37705809619668e-13;
    constexpr double C6 =  1.60484140302171e-10;
    constexpr double C5 = -2.5051890975381e-08;
    constexpr double C4 =  2.75573167776257e-06;
    constexpr double C3 = -0.000198412698265406;
    constexpr double C2 =  0.00833333333329128;
    constexpr double C1 = -0.166666666666663;

    const double x = y.d;
    const double xx = y.d * y.d;
    y.d = std::fma(C7, xx, C6);
    y.d = std::fma(y.d, xx, C5);
    y.d = std::fma(y.d, xx, C4);
    y.d = std::fma(y.d, xx, C3);
    y.d = std::fma(y.d, xx, C2);
    y.d = std::fma(y.d, xx, C1);
    y.d = std::fma(y.d * xx, x, x);

    return y.d;
}

constexpr uint64_t max_frac(uint64_t frac_bits)
{
    return ((uint64_t)1 << frac_bits) - 1;
}

double ChowChow::amp_linear(uint64_t phase)
{
    constexpr uint64_t MAX_FRAC = max_frac(FRAC_BITS);

    uint64_t y0ndx = phase >> FRAC_BITS;
    double fract = static_cast<double>(phase & MAX_FRAC)/MAX_FRAC;

    uint64_t y1ndx = y0ndx + 1;
    if (y1ndx > SINE_TAB_LAST) y1ndx = 0;

    const auto y0  = SINE_TAB[y0ndx];
    const auto y1  = SINE_TAB[y1ndx];

    return (y0 + (y1 - y0)*fract);
}

double ChowChow::amp_raw_lookup(uint64_t phase)
{
    return SINE_TAB[phase >> FRAC_BITS];
}

double ChowChow::amp_circ(uint64_t phase)
{
    constexpr int SMALL_NDX_BITS = 6;
    constexpr int FRAC_BITS_P_SMALL_NDX_BITS = FRAC_BITS;
    constexpr int FRAC_BITS = FRAC_BITS_P_SMALL_NDX_BITS - SMALL_NDX_BITS;
    constexpr uint64_t MAX_FRAC = max_frac(FRAC_BITS);
    constexpr double ONED_MAX_FRAC = 1./MAX_FRAC;

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

/* std::fmod is actually faster under MSVC than this shortcut.
 * This may change in the future, so remember that.
 */
#ifndef _MSC_VER
double pow2_fmod4(union double_and_bits db)
{
    const double x = db.d;

    constexpr uint_fast16_t COEFF_BITS = 52;
    constexpr uint_fast16_t EXPON_MASK = 0x7ff;
    uint_fast16_t e = db.l >> COEFF_BITS & EXPON_MASK;

    constexpr uint_fast16_t EXP_4_M1 = 1024;
    e = COEFF_BITS - (e - EXP_4_M1);

    db.l = db.l >> e << e;

    return x - db.d;
}
#endif

double P::amp(double mod) const
{
    union double_and_bits db;
    db.d = mod;

    if ((db.i[1] & 0x7ff00000) > 0x40100000) { /* mod > 4.0 */
#ifdef _MSC_VER
        mod = std::fmod(mod, 4.0);
#else
        mod = pow2_fmod4(db);
#endif
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

P::SineAlg P::acc_to_sine_alg(std::size_t acc)
{
    return magic_enum::enum_value<SineAlg>(acc - 1);
}

P::amp_fn_t P::amp_fn_ptr(SineAlg alg) const
{
    switch (alg) {
        case (SineAlg::taylor):     return *amp_taylor;
        case (SineAlg::circular):   return *amp_circ;
        case (SineAlg::linear):     return *amp_linear;
        case (SineAlg::raw_lookup): return *amp_raw_lookup;
        default:                    return DEFAULT_SINE_FN;
    }
}

std::string P::sine_alg_id(SineAlg alg)
{
    switch (alg) {
        case (PhaseAcc::SineAlg::raw_lookup): return "raw lookup";
        case (PhaseAcc::SineAlg::linear):     return "linear";
        case (PhaseAcc::SineAlg::circular):   return "circular";
        case (PhaseAcc::SineAlg::taylor):     return "taylor";
        default:                              return "unknown";
    }
}

std::string P::sine_alg_id(std::size_t acc)
{
    return sine_alg_id(acc_to_sine_alg(acc));
}

P::SineAlg P::sine_alg() const
{
    if (amp_fn == *amp_taylor) {
        return SineAlg::taylor;
    } else if (amp_fn == *amp_circ) {
        return SineAlg::circular;
    } else if (amp_fn == *amp_linear) {
        return SineAlg::linear;
    } else if (amp_fn == *amp_raw_lookup) {
        return SineAlg::raw_lookup;
    } else {
        std::cerr << "WARNING: You asked for the current sine alg, but the "
                     "function in use does not correspond to any of the "
                     "known algorithms. Something is probably wrong. "
                     "The address of the function in use is "
                  << std::hex << (void*)amp_fn << std::dec
                  << ". Returning the default, although that's probably not "
                     "correct.";

        return DEFAULT_SINE_ALG;
    }
}

void P::sine_alg(SineAlg alg)
{
    amp_fn = amp_fn_ptr(alg);
}

void P::sine_alg(std::size_t acc)
{
    if (acc <= SINE_ALG_COUNT && acc > 0) {
        sine_alg(acc_to_sine_alg(acc));
    } else {
            std::cerr << "WARNING: '"
                      << acc
                      << "' does not correspond to a sine algorithm."
                         " Setting to the default.";
            amp_fn = DEFAULT_SINE_FN;
    }
}
