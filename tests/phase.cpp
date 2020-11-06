#include <doctest.h>
#include <cmath>
#include <iostream>

#include "chow-chow/phase.hpp"

using namespace ChowChow;

//void to_double_test(Phase p, long double expected)
//{
//    CHECK(std::abs((p.phase_d() - expected)) < (long double)1e-18);
//}

//TEST_CASE("phase -> double") {
//    to_double_test(Phase {Phase::PI}, Phase::LONG_D_PI);
//
//    to_double_test(Phase {Phase::PI_N}, -Phase::LONG_D_PI);
//
//    CHECK(std::abs(Phase {Phase::ZERO}.phase_d()) == 0.0);
//
//    to_double_test(Phase {Phase::PI_D2}, Phase::LONG_D_H_PI);
//
//    to_double_test(Phase {Phase::PI_ND2}, -Phase::LONG_D_H_PI);
//}
//
//TEST_CASE("double -> phase") {
//    CHECK((Phase {Phase::LONG_D_PI}).phase() == Phase::PI);
//
//    CHECK((Phase {-Phase::LONG_D_PI}).phase() == Phase::PI_N);
//
//    CHECK(Phase {(long double)0.0}.phase() == Phase::ZERO);
//
//    CHECK((Phase {Phase::LONG_D_H_PI}).phase() == Phase::PI_D2);
//
//    CHECK((Phase {-Phase::LONG_D_H_PI}).phase() == Phase::PI_ND2);
//
//    CHECK((Phase {Phase::LONG_D_H_SQ_3_PI}).phase() == (Phase::PI_D2 * std::sqrt(3.0L)));
//
//    CHECK((Phase {-Phase::LONG_D_H_SQ_3_PI}).phase() == (Phase::PI_ND2 * std::sqrt(3.0L)));
//}

TEST_CASE("phase arithmetic") {
    Phase a {Phase::ZERO};
    Phase b {Phase::PI_D2};

    CHECK(a + b == (Phase::ZERO + Phase::PI_D2));
    CHECK(a - b == (Phase::ZERO - Phase::PI_D2));

    CHECK(a != b);
}

TEST_CASE("phase wraparound") {
    Phase a {Phase::PI};
    a += 1;

    CHECK(a == Phase::PI_N);
}

TEST_CASE("phase -> amp") {
    SUBCASE("0") {
        constexpr Phase p {Phase::ZERO};
        constexpr Amp a {0};

        CHECK(p.amp() == a);
    }

    SUBCASE("pi") {
        constexpr Phase p {Phase::PI};
        constexpr Amp a {Amp::PI};

        CHECK(p.amp() == a);
    }

    SUBCASE("-pi") {
        constexpr Phase p {Phase::PI_N};
        constexpr Amp a {-Amp::PI};

        CHECK(p.amp() == a);
    }

    SUBCASE("pi/2") {
        constexpr Phase p {Phase::PI_D2};
        constexpr Amp a {Amp::PI/2};

        CHECK(p.amp() == a);
    }

    SUBCASE("-pi/2") {
        constexpr Phase p {Phase::PI_ND2};
        constexpr Amp a {-Amp::PI/2};

        CHECK(p.amp() == a);
    }

    SUBCASE("pi/6") {
        constexpr Phase p {(Phase::PI/12)*7};
        constexpr Amp a {Amp::PI/6};

        CHECK(p.amp() == a);
    }

    SUBCASE("-pi/6") {
        constexpr Phase p {(Phase::PI/12)*5};
        constexpr Amp a {-Amp::PI/6};

        CHECK(p.amp() == a);
    }

    SUBCASE("pi*7/8") {
        constexpr Phase p {(Phase::PI/16)*15};
        constexpr Amp a {Amp::PI*7/8};

        CHECK(p.amp() == a);
    }

    SUBCASE("-pi*7/8") {
        constexpr Phase p {Phase::PI/16};
        constexpr Amp a {-Amp::PI*7/8};

        CHECK(p.amp() == a);
    }
}

TEST_CASE("double -> phase") {
    double frac = 0.25;
    Phase f {frac};
    Phase p;

    p += f;

    CHECK((p.phase() - Phase::PI_D2) <= 2);

    p += f;

    CHECK(p.phase() == Phase::PI);
}

TEST_CASE("phase -> double") {
    double frac = 0.25;
    Phase p {frac};

    CHECK(std::abs(p.phase_d() - frac) < 0.0000000000000000001);
}

TEST_CASE("[-pi, pi] output") {

    Phase p {Phase::PI};

    CHECK(p.phase_piscale() == Phase::LONG_D_PI);
}
