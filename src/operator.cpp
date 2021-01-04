#include <cmath>

#include "chow-chow/constants.hpp"
#include "chow-chow/operator.hpp"
#include "chow-chow/sine_tab.hpp"

using namespace ChowChow;

double Operator::sig(double mod) const
{
    return phase_acc.amp(mod);
}

double Operator::sig() const
{
    return phase_acc.amp();
}

void Operator::advance()
{
    if (vibr_on) {
        phase_acc.advance(vibr);
        vibr.advance();
    } else {
        phase_acc.advance();
    }
}

void Operator::reset_frq()
{
    phase_acc.frequency((frq + frq_offset) * rtio);
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
        if (!vibr_on) {
            vibr_on = true;
            vibr.reset_phase();
        }
        vibr.output_amp(n * 10);
    }
}

void Operator::index(double n)
{
    phase_acc.output_amp(n);
}

void Operator::sample_rate(uint64_t rate)
{
    phase_acc.sample_rate(rate);
}
