#include <cmath>
#include <string>
#include <sstream>
#include <stdexcept>
#include <iostream>

#include "chow-chow/frequency.hpp"

using F = ChowChow::Frequency;

template<class T>
F::freq_t shifted_integ(T n)
{
    return static_cast<F::freq_t>(n) << F::FREQ_FRAC_BITS;
}

F::Frequency(double frq)
{
    double integ;
    double frac = std::modf(frq, &integ);

    f = shifted_integ(integ) + static_cast<freq_t>(MAX_FRAC * frac);
}

F::Frequency(unsigned int fr)
{
    f = shifted_integ(fr);
}

double F::make_double() const
{
    return static_cast<double>(intg())
           + (static_cast<double>(frac()) / MAX_FRAC);
}

F& F::operator*=(Frequency fr)
{
    f = (static_cast<__uint128_t>(f) * static_cast<__uint128_t>(fr.raw()))
        >> FREQ_FRAC_BITS;

    return *this;
}
