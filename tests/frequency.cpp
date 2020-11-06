#include <cmath>
#include <stdexcept>

#include <doctest.h>

#include <chow-chow/frequency.hpp>

using namespace ChowChow;

TEST_CASE("Frequency - integral") {
    constexpr Frequency::freq_t i = 2034;
    constexpr Frequency::freq_t f = 12034;

    SUBCASE("init") {
        const Frequency freq {i, f};

        CHECK(freq.intg() == i);
        CHECK(freq.frac() == f);
    }

    SUBCASE("get/set") {
        Frequency freq;
        freq.intg(i);
        freq.frac(f);

        CHECK(freq.intg() == i);
        CHECK(freq.frac() == f);
    }
}

TEST_CASE("Frequency - floating point") {
    constexpr double i = 4920;
    constexpr Frequency::freq_t f_int = 4923;
    constexpr double f = static_cast<double>(f_int) / Frequency::FREQ_MAX;
    constexpr double d = i + f;

    SUBCASE("init") {
        const Frequency frq {d};

        CHECK(frq.frac() == f_int);
        CHECK(frq.freq() == d);
        CHECK(frq.intg() == i);
        CHECK(frq.frac_f() == f);
    }

    SUBCASE("get/set") {
        Frequency frq;
        frq.freq(d);

        CHECK(frq.freq() == d);
    }
}

TEST_CASE("invalid freq args") {
    Frequency frq;

    SUBCASE("NaN") {
        CHECK_THROWS(frq.freq(std::nan("0")));
    }

    SUBCASE("below min") {
        frq.freq(-1923.);
        CHECK(frq.freq() == Frequency::FREQ_MIN);
    }

    SUBCASE("above max") {
        frq.freq(static_cast<double>(Frequency::FREQ_MAX) + 1302);
        CHECK(frq.freq() == Frequency::FREQ_MAX);
    }

}
