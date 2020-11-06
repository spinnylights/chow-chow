#include <cmath>
#include <string>
#include <sstream>
#include <stdexcept>
#include <iostream>

#include "chow-chow/frequency.hpp"

using F = ChowChow::Frequency;

void freq_arg_err(const std::string kind, const double arg)
{
    std::stringstream s;
    s << kind << " frequencies are not permitted; you passed " << arg << " .\n";
    throw std::invalid_argument(s.str());
}

void freq_arg_sat(const std::string kind, const double lim, double& arg)
{

    std::cerr << "WARNING: The " << kind << " supported frequency is " << lim
              << "; you passed " << arg << ". "
              << "Saturating at " << lim << ".\n";
    arg = lim;
}

F::Frequency(double frq) { freq(frq); }

double F::freq() const
{
    return static_cast<double>(i) + frac_f();
}

double F::frac_f() const
{
   return static_cast<double>(f) / FREQ_MAX;
}

void F::freq(double frq)
{
    if (std::isnan(frq)) { freq_arg_err("NaN", frq); }
    if (frq > FREQ_MAX) { freq_arg_sat("max", FREQ_MAX, frq); }
    if (frq < FREQ_MIN) { freq_arg_sat("min", FREQ_MIN, frq); }

    double ipart;
    const double fpart = std::modf(frq, &ipart);

    i = static_cast<freq_t>(ipart);
    f = static_cast<freq_t>(std::round(fpart * FREQ_MAX));
}
