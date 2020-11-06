#ifndef odd10ca20e8c4c1d90cc4cfef8b0e13e
#define odd10ca20e8c4c1d90cc4cfef8b0e13e

#include <cstdint>

namespace ChowChow {
    class Frequency {
    public:
        using freq_t = uint16_t;
        static constexpr freq_t FREQ_MAX = UINT16_MAX;
        static constexpr freq_t FREQ_MIN = 0;

        Frequency() {};
        Frequency(freq_t integ, freq_t frct) :i{integ}, f{frct} {}
        Frequency(double f);

        constexpr freq_t intg() const { return i; }
        constexpr freq_t frac() const { return f; }
        double freq() const;
        double frac_f() const;

        void intg(freq_t integ) { i = integ; }
        void frac(freq_t frct) { f = frct; }
        void freq(double f);

    private:
        uint16_t i = FREQ_MIN;
        uint16_t f = FREQ_MIN;
    };
}

#endif
