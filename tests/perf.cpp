#include <cstdint>
#include <fstream>
#include <chrono>
#include <array>
#include <iostream>
#include <iomanip>
#include <random>

#include "chow-chow/operators.hpp"

namespace t = std::chrono;

using namespace ChowChow;

static constexpr size_t SAMPLE_RATE = 48000;
static constexpr size_t LENGTH_SECS = 2;
static constexpr size_t LENGTH = SAMPLE_RATE * LENGTH_SECS;

static constexpr double VIB_FRQ = 40.0;
static constexpr double VIB_AMP = 15.25;

static constexpr size_t OPS = 3;

class ProfRun {
public:
    ProfRun(std::size_t sine_alg_acc)
    {
        ramp = [&]{
            std::vector<double> e;

            for (size_t i = 0; i < LENGTH; ++i) {
                const double x = static_cast<double>(i) / LENGTH;
                e.push_back(1.0 - x);
            }

            return e;
        }();

        const auto before = t::steady_clock::now();

        ops.sample_rate(SAMPLE_RATE);

        ops.sine_alg(sine_alg_acc);

        fn = PhaseAcc::sine_alg_id(sine_alg_acc);

        static constexpr double FREQ = 250.; // ~d2

        ops[3].freq(FREQ);
        ops[2].freq(FREQ);
        ops[1].freq(FREQ);

        ops[3].ratio(2.35);
        ops[2].ratio(3.85);

        ops.connect(3, 2, 2.3);
        ops.connect(2, 1, 1.8);

        ops.output(1);

        setup = t::steady_clock::now() - before;
    }

    void run()
    {
        // so our profiling run doesn't get optimized out
        std::vector<double> dummy;

        for (std::size_t i = 0; i < LENGTH; ++i) {

            ops[2].index(0.25 + ramp[i]);

            dummy.push_back(get_sig());
        }

#ifdef _WIN32
        static const std::string NULL_DEV = "NUL:";
#else
        static const std::string NULL_DEV = "/dev/null";
#endif

        std::ofstream dev_null(NULL_DEV);
        dev_null << dummy.back();

    }

    void print_results()
    {
        std::cout << "\n" << "sine alg: " << fn << "\n";

        std::cout << std::fixed;
        std::cout << std::setprecision(9);

        print_time(setup,    "setup:   ");
        print_time(runtime,  "runtime: ");
    }

private:
    double get_sig()
    {
        const auto before_run = t::steady_clock::now();
        const auto sig = ops.sig();
        runtime += t::steady_clock::now() - before_run;
        return sig;
    }

    void print_time(t::steady_clock::duration d, std::string preface)
    {
        std::cout << preface << t::duration<double>(d).count() << "s\n";
    }

    Operators<OPS> ops;
    t::steady_clock::duration setup = {};
    t::steady_clock::duration runtime = {};
    std::vector<double> ramp;
    std::string fn;
};

int main(void)
{
    std::cout << "** PERF TEST **\n";

    auto rd = std::random_device {};
    auto rng = std::default_random_engine {rd()};

    std::array<size_t, PhaseAcc::SINE_ALG_COUNT> indxs;
    for (size_t i = 0; i < indxs.size(); ++i) {
        indxs[i] = i;
    }

    std::vector<ProfRun> ps;
    for (const auto& i : indxs) {
        std::size_t acc = i + 1;
        ps.push_back({acc});
    }

    for (size_t i = 0; i < 30; ++i) {
        std::shuffle(indxs.begin(), indxs.end(), rng);

        for (const auto& indx : indxs) {
            ps.at(indx).run();
        }
    }

    for (auto& p : ps) {
        p.print_results();
    }
}
