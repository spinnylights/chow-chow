#include <doctest.h>

#include "chow-chow/sample.hpp"

using namespace ChowChow;

TEST_CASE("Sample") {
    SUBCASE("init with int sets it") {
        constexpr Sample::smp_t n = 12105;
        const Sample s {n};
        CHECK(s.val() == n);
    }

    SUBCASE("init with long double scales it") {
        constexpr long double n = -0.219;
        const int16_t expected = std::round(Sample::SMP_MAX_NEG * n);
        const Sample s {n};
        CHECK(s.val() == expected);
    }

    SUBCASE("setting amp via long double scales it") {
        constexpr long double a = 0.5;
        const int16_t expected = static_cast<int16_t>(a * Sample::AMP_MAX);
        Sample s;
        s.amp(a);
        CHECK(s.amp() == expected);
    }

    SUBCASE("long double amp scales val") {
        constexpr Sample::smp_t n = 19210;
        constexpr long double a = 0.5;
        const int16_t expected = static_cast<int16_t>(n * a);
        Sample s {n};
        s.amp(a);
        CHECK(s.val() == expected);
    }

    SUBCASE("setting amp via int") {
        constexpr Sample::amp_t a = 2190;
        Sample s;
        s.amp(a);
        CHECK(s.amp() == a);
        CHECK(s.amp_f() == static_cast<long double>(a) / Sample::AMP_MAX);
    }

    SUBCASE("setting neg val via int") {
        constexpr Sample::smp_t n = -1239;
        Sample s {n};
        CHECK(s.val() == n);
        CHECK(s.val_f() == static_cast<long double>(n) / Sample::SMP_MAX_NEG);
    }

    SUBCASE("setting pos val via int") {
        constexpr Sample::smp_t n = 1239;
        Sample s {n};
        CHECK(s.val() == n);
        CHECK(s.val_f() == static_cast<long double>(n) / Sample::SMP_MAX_POS);
    }
}
