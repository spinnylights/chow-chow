#include <doctest.h>

#include <vector>
#include <string>

#include "chow-chow/phase_acc.hpp"

using namespace ChowChow;

void simple_advancing_test(PhaseAcc::SineAlg alg,
                           std::string msg,
                           double epsilon = 1e-14)
{
    static constexpr double PI_4 = 0.7071067811865475;

    std::vector<double> expecteds =
        {0.0, PI_4, 1.0, PI_4, 0.0, -PI_4, -1.0, -PI_4};

    static const uint64_t sr = 800;
    static const double freq = 1;

    PhaseAcc p = {freq, sr};

    p.sine_alg(alg);

    CHECK_MESSAGE(p.phase_incr() ==
                      freq * static_cast<double>(PhaseAcc::TAU) / sr,
                  msg);

    auto advance = [&]{
        for (int i = 0; i < sr / expecteds.size(); ++i) {
            p.advance();
        }
    };

    for (std::size_t i = 0; i < expecteds.size(); ++i) {
        CHECK_MESSAGE(p.amp() - expecteds[i] < epsilon,
                      msg);

        advance();
    }
}

TEST_CASE("simple advancing") {
    simple_advancing_test(PhaseAcc::SineAlg::raw_lookup,
                          "raw lookup",
                          1e-2);
    simple_advancing_test(PhaseAcc::SineAlg::linear, "linear");
    simple_advancing_test(PhaseAcc::SineAlg::circular, "circular");
    simple_advancing_test(PhaseAcc::SineAlg::taylor, "taylor");
}

TEST_CASE("cardinal interface to sine_alg") {
    PhaseAcc p {440, 480000};

    auto amp = p.amp();

    for (auto i =  PhaseAcc::SINE_ALG_START;
         i      <= PhaseAcc::SINE_ALG_COUNT;
         ++i) {
        p.sine_alg(i);

        PhaseAcc::SineAlg alg = PhaseAcc::acc_to_sine_alg(i);
        CHECK(p.sine_alg() == alg);

        p.advance();
        auto new_amp = p.amp();
        CHECK(amp != new_amp);
        amp = new_amp;
    }
}

TEST_CASE("sine alg to string") {
    for (auto i =  PhaseAcc::SINE_ALG_START;
         i      <= PhaseAcc::SINE_ALG_COUNT;
         ++i) {
        auto acc_str = PhaseAcc::sine_alg_id(i);

        PhaseAcc::SineAlg alg = PhaseAcc::acc_to_sine_alg(i);
        auto alg_str = PhaseAcc::sine_alg_id(alg);

        CHECK(acc_str == alg_str);
    }
}

TEST_CASE("advancing with modulator") {
    static const Frequency fr = 8;
    static const uint64_t sr = 64;

    PhaseAcc p = {fr, sr};

    CHECK(p.amp() == 0.);

    CHECK(p.amp(1.0) == 1.0);

    p.advance();

    CHECK(p.amp(0.5) == 1.0);
    CHECK(p.amp(-0.5) - 0.0 < 1e-15);
}
