#include <doctest.h>

#include <vector>

#include "chow-chow/phase_acc.hpp"

using namespace ChowChow;

TEST_CASE("simple advancing") {
    static constexpr double PI_4 = 0.7071067811865475;

    std::vector<double> expecteds =
        {0.0, PI_4, 1.0, PI_4, 0.0, -PI_4, -1.0, -PI_4};

    static const PhaseAcc::phase_t sr = 800;
    static const double freq = 1;

    PhaseAcc p = {freq, sr};

    CHECK(p.phase_incr() == freq * static_cast<double>(PhaseAcc::TAU) / sr);

    auto advance = [&]{
        for (int i = 0; i < sr / expecteds.size(); ++i) {
            p.advance();
        }
    };

    for (std::size_t i = 0; i < expecteds.size(); ++i) {
        CHECK(p.amp() - expecteds[i] < 1e-14);

        advance();
    }
}
