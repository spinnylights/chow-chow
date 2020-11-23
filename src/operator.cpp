#include <cmath>

#include "chow-chow/constants.hpp"
#include "chow-chow/operator.hpp"
#include "chow-chow/sine_tab.hpp"

using namespace ChowChow;

double Operator::sig() const
{
    auto amp = phase.amp();

    //for (const auto& m : mods) {
    //    amp *= m.mod->sig() * m.amp;
    //}

    return amp;
}

void Operator::advance()
{
    if (vibr_on) {
        phase.advance(vibr);
    } else {
        phase.advance();
    }

    for (const auto& m : mods) {
        phase += m.mod->sig() * m.amp;
    }
}

void Operator::reset_frq()
{
    phase.frequency((frq + frq_offset) * rtio);
}

void Operator::freq(double n)
{
    frq = n;
    reset_frq();
}

void Operator::freq_offset(double n)
{
    frq_offset = n * 0.001;
    reset_frq();
}

void Operator::ratio(double n)
{
    rtio = n;
    reset_frq();
}

void Operator::vibrato_freq(double n)
{
    vibr.frequency(n);
}

void Operator::vibrato_amp(double n)
{
    if (n == 0.) {
        vibr_on = false;
    } else {
        vibr_on = true;
        vibr.output_amp(n * 0.0001);
    }
}

void Operator::index(double n)
{
    phase.output_amp(n / 100.);
}

void Operator::sample_rate(PhaseAcc::phase_t rate)
{
    phase.sample_rate(rate);
}

void Operator::add_modulator(const Operator& op, PhaseAcc::amp_t amp)
{
    mods.push_back({&op, amp});
}

void Operator::clear_modulators()
{
    mods.clear();
}
