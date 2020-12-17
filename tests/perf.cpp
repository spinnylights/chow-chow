#include <cstdint>
#include <fstream>
#include <chrono>
#include <array>
#include <iostream>
#include <iomanip>

#include "chow-chow/operators.hpp"

namespace t = std::chrono;

using namespace ChowChow;

static constexpr size_t SAMPLE_RATE = 48000;
static constexpr size_t LENGTH_SECS = 20;
static constexpr size_t LENGTH = SAMPLE_RATE * LENGTH_SECS;

class ProfRun {
public:
    ProfRun(PhaseAcc::SineAlg sfn)
        :fn{sfn}
    {
        const auto before = t::steady_clock::now();

        ops.sample_rate(SAMPLE_RATE);

        ops.sine_alg(fn);

        ops[10].freq(13126.974048651);
        ops[9].freq(14144.88951279354);
        ops[8].freq(2898.3222486121517);
        ops[7].freq(2878.412085879245);
        ops[6].freq(5464.611331928606);
        ops[5].freq(15509.342454971022);
        ops[4].freq(488.9778654984108);
        ops[3].freq(16775.10809601692);
        ops[2].freq(18661.19102658915);
        ops[1].freq(17664.31544642939);

        ops[8].ratio(5.93704982759879);
        ops[9].ratio(8.638477916782952);
        ops[8].ratio(4.906882512632615);
        ops[7].ratio(4.566359535717398);
        ops[6].ratio(0.42219593808689515);
        ops[5].ratio(0.8915818065106733);
        ops[4].ratio(5.929188029502441);
        ops[3].ratio(3.2890648603934722);
        ops[2].ratio(5.370235031798406);
        ops[1].ratio(9.02961705374125);

        ops[10].index(1.1692492545429838);
        ops[9].index(2.093129103156454);
        ops[8].index(7.480729164102332);
        ops[7].index(2.156953055634912);
        ops[6].index(1.1899807519294092);
        ops[5].index(1.244709379963963);
        ops[4].index(1.1065222424812804);
        ops[3].index(2.1188492043919456);
        ops[2].index(8.621453890817373);
        ops[1].index(70.20500272644726);

        ops.connect(10, 9);
        ops.connect(9, 8);
        ops.connect(8, 7);
        ops.connect(7, 6);
        ops.connect(6, 5);
        ops.connect(5, 4);
        ops.connect(4, 3);
        ops.connect(3, 2);
        ops.connect(2, 1);
        ops.connect(1, 10);

        ops.output(1);

        ops.reorder();

        setup = t::steady_clock::now() - before;
    }

    void run()
    {
        // so our profiling run doesn't get optimized out
        std::vector<double> dummy(LENGTH);

        for (std::size_t i = 0; i < LENGTH; ++i) {
            dummy.push_back(get_sig(ops));
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
        std::cout << "\n";
        std::cout << "sine alg: ";

        switch (fn) {
            case (PhaseAcc::SineAlg::raw_lookup): std::cout << "raw lookup";
                                                   break;
            case (PhaseAcc::SineAlg::linear):     std::cout << "linear";
                                                   break;
            case (PhaseAcc::SineAlg::circular):   std::cout << "circular";
                                                   break;
            case (PhaseAcc::SineAlg::stdlib):     std::cout << "stdlib";
                                                   break;
        }

        std::cout << "\n";

        std::cout << std::fixed;
        std::cout << std::setprecision(9);

        print_time(setup,    "setup:   ");
        print_time(runtime,  "runtime: ");
    }

private:
    double get_sig(Operators<10> ops)
    {
        const auto before_run = t::steady_clock::now();
        const auto sig = ops.sig();
        ops.advance();
        runtime += t::steady_clock::now() - before_run;
        return sig;
    }

    void print_time(t::steady_clock::duration d, std::string preface)
    {
        std::cout << preface << t::duration<double>(d).count() << "s\n";
    }

    Operators<10> ops;
    t::steady_clock::duration setup = {};
    t::steady_clock::duration runtime = {};
    PhaseAcc::SineAlg fn;
};

int main(void)
{
    std::cout << "** PERF TEST **\n";

    std::array<PhaseAcc::SineAlg, 4> sfns =
        { PhaseAcc::SineAlg::raw_lookup,
          PhaseAcc::SineAlg::linear,
          PhaseAcc::SineAlg::circular,
          PhaseAcc::SineAlg::stdlib, };

    for (const auto& sfn : sfns) {
        ProfRun r = {sfn};
        r.run();
        r.print_results();
    }
}
