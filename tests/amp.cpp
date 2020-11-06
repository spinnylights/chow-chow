#include <doctest.h>

#include <iostream>
#include <iomanip>
#include <string>

#include "chow-chow/amp.hpp"

using namespace ChowChow;

TEST_CASE("default amp constructor matches") {
    const Amp a {};
    const Amp b {};

    CHECK(a == b);
}

TEST_CASE("two amps init with same val match") {
    constexpr Amp::amp_t n = 3;

    const Amp a {n};
    const Amp b {n};

    CHECK(a == b);
}

TEST_CASE("get/set amp") {
    Amp::amp_t i = 3;
    Amp a {i};

    CHECK(a.amp() == i);

    Amp::amp_t j = 5;
    a.amp(j);

    CHECK(a.amp() == j);
}

TEST_CASE("amp -> string") {
    std::string s = "170141183460469231731687303715884105727";

    CHECK(Amp {Amp::AMP_MAX}.to_s() == s);
}

TEST_CASE("double -> amp") {
    constexpr long double pi = 3.14159265358979;
    constexpr Amp::amp_t expected = 3141592653589790007;
    Amp a {pi};

    CHECK(a == expected);
}

TEST_CASE("amp arithmetic") {
    constexpr double x = 8.;
    constexpr double y = 2.;
    constexpr double add = x + y;
    constexpr double sub = x - y;
    constexpr double mul = x * y;

    Amp a {x};
    Amp b {y};

    SUBCASE("inequalities") {
        CHECK(a != b);
        CHECK(a > b);
        CHECK(a >= b);
        CHECK(b < a);
        CHECK(b <= a);
    }

    SUBCASE("+") { CHECK(a + b == add); }
    SUBCASE("+=") {
        Amp c {a};
        CHECK((c += b) == add);
    }

    SUBCASE("-") { CHECK(a - b == sub); }
    SUBCASE("-=") {
        Amp c {a};
        CHECK((c -= b) == sub);
    }

    SUBCASE("*") { CHECK(a * b == mul); }
    SUBCASE("*=") {
        Amp c {a};
        CHECK((c *= b) == mul);
    }
}
