#include <fstream>
#include <chrono>
#include <array>
#include <iostream>
#include <iomanip>

#include "chow-chow/operators.hpp"

namespace t = std::chrono;

using namespace ChowChow;

static constexpr size_t SAMPLE_RATE = 48000;
static constexpr size_t LENGTH_SECS = 10;
static constexpr size_t LENGTH = SAMPLE_RATE * LENGTH_SECS;

void setup(Operators<10>& ops)
{
    ops.sample_rate(SAMPLE_RATE);

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
}

double get_sig(Operators<10> ops)
{
    const auto sig = ops.sig();
    ops.advance();
    return sig;
}

void print_time(t::steady_clock::duration d, std::string preface)
{
    std::cout << preface << t::duration<double>(d).count() << "s\n";
}

int main(void)
{
    Operators<10> ops;

    const auto before_setup = t::steady_clock::now();
    setup(ops);
    const auto setup_time = t::steady_clock::now() - before_setup;

    // so our profiling run doesn't get optimized out
    std::array<double, LENGTH> dummy;

    const auto before_run = t::steady_clock::now();
    for (std::size_t i = 0; i < LENGTH; ++i) {
        dummy[i] = get_sig(ops);
    }
    const auto runtime = t::steady_clock::now() - before_run;

#if defined(_WIN32)
    static const std::string NULL_DEV = "NUL:";
#else
    static const std::string NULL_DEV = "/dev/null";
#endif

    std::ofstream dev_null(NULL_DEV);
    dev_null << *dummy.end();

    std::cout << std::fixed;
    std::cout << std::setprecision(15);
    print_time(setup_time, "setup:   ");
    print_time(runtime,    "runtime: ");
}
