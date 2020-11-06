#ifndef O305296574064f64b1b173eb547f7067
#define O305296574064f64b1b173eb547f7067

#include <array>
#include <cmath>

#include "chow-chow/sample.hpp"

namespace ChowChow {
    static constexpr double PI = 3.141592653589793;

    static constexpr double appr_sine(const double x)
    {
        return x -
               ((x*x*x) / (3*2)) +
               ((x*x*x*x*x) / (5*4*3*2)) -
               ((x*x*x*x*x*x*x) / (7*6*5*4*3*2)) +
               ((x*x*x*x*x*x*x*x*x) / (9*8*7*6*5*4*3*2)) -
               ((x*x*x*x*x*x*x*x*x*x*x) / (11*10*9*8*7*6*5*4*3*2)) +
               ((x*x*x*x*x*x*x*x*x*x*x*x*x) /
                ((uint64_t)13*12*11*10*9*8*7*6*5*4*3*2)) -
               ((x*x*x*x*x*x*x*x*x*x*x*x*x*x*x) /
                ((uint64_t)15*14*13*12*11*10*9*8*7*6*5*4*3*2));
    }

    static constexpr Sample::smp_t s_round(const double n)
    {
        const auto ni = static_cast<Sample::smp_t>(n);
        const double nd = n - ni;

        if (n < 0.) {
            if (nd < -0.5) {
                return ni - 1;
            } else {
                return ni;
            }

        } else {
            if (nd < 0.5) {
                return ni;
            } else {
                return ni + 1;
            }
        }
    }

    static constexpr auto SINE_TAB_LEN = 32768;

    static constexpr auto SINE_TAB = []
    {
        std::array<Sample::smp_t, SINE_TAB_LEN> s = {};

        for (size_t i = 0; i < SINE_TAB_LEN; ++i) {
            double v = 0.;
            constexpr auto HALFWAY = SINE_TAB_LEN / 2;
            const auto di = static_cast<double>(i);
            if (i < HALFWAY) {
                v = (1. - (di / HALFWAY)) * PI;
                s[i] = s_round(appr_sine(v) * Sample::SMP_MAX_POS);
            } else if (i > HALFWAY) {
                v = ((di - HALFWAY) / HALFWAY) * -PI;
                s[i] = s_round(appr_sine(v) * Sample::SMP_MAX_NEG);
            }
        }

        return s;
    }();
}

#endif
