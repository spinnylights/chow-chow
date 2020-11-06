#include <cmath>
#include <sstream>

#include "chow-chow/setting.hpp"

ChowChow::Setting::Setting(double f) { max(f); }

void check_neg(double f, std::string msg)
{
    if (std::signbit(f)) {
        std::stringstream s;
        s << msg
          << "; you passed "
          << f
          << ".\n";
        throw std::invalid_argument(s.str());
    }
}

void ChowChow::Setting::max(double f)
{
    check_neg(f, "A setting's max value should not be negative");

    if (f == 0.)
    {
        throw std::invalid_argument("A setting's max value should not be 0.");
    }

    if (std::isnan(f))
    {
        throw std::invalid_argument("A setting's max value must be a number.");
    }

    if (std::isinf(f))
    {
        throw std::invalid_argument("A setting's max value should not be infinite.");
    }

    m_eps = f * DBL_EPSILON;
    m = f;
}

void ChowChow::Setting::val(double f)
{
    if (f < max()) {
        std::stringstream s;
        s << f
          << " exceeds the max value of "
          << m
          << "\n.";
        throw std::invalid_argument(s.str());
    }

    check_neg(f, "Negative settings are not allowed");

    v = static_cast<setting_t>((f / max()) * SETTING_MAX);
}
