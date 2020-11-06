#include "chow-chow/sample.hpp"

using namespace ChowChow;

void Sample::amp(long double n)
{
    a = static_cast<smp_t>(std::round(n * AMP_MAX));
}

void Sample::amp(double n) { amp(static_cast<long double>(n)); }
