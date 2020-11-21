#include <cmath>

#include "chow-chow/constants.hpp"
#include "chow-chow/operator.hpp"

using namespace ChowChow;

double Operator::wave(double theta) const
{
    const double freq_base = (frq + vibrato(theta)) * rtio;

    return freq_base + freq_base * frq_offset;
}

double Operator::vibrato(double theta) const
{
    if (vibr_amp == 0. || vibr_freq == 0.) {
        return 0;
    } else {
        return frq * vibr_amp * std::sin(vibr_freq * theta * TAU);
    }
}

double Operator::sig(double theta, double mod) const
{
    return std::sin(wave(theta) * theta * TAU + mod) * ndx;
}

void Operator::freq(double n)
{
    constexpr double FREQ_MIN = 20.;
    constexpr double FREQ_MAX = 20000.;

    if (n < FREQ_MIN) { n = FREQ_MIN; }

    if (n > FREQ_MAX) { n = FREQ_MAX; }

    frq = n;
}

void Operator::freq_offset(double n)
{
    frq_offset = n * 0.001;
}

void Operator::ratio(double n) { rtio = n; }

void Operator::vibrato_freq(double n)
{
    constexpr double FREQ_MAX = 19.;

    if (n > FREQ_MAX) { n = FREQ_MAX; }

    vibr_freq = n;
}

void Operator::vibrato_amp(double n)
{
    vibr_amp = n * 0.0001;
}

void Operator::index(double n) { ndx = n; }
