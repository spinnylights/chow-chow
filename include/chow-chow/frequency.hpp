#ifndef odd10ca20e8c4c1d90cc4cfef8b0e13e
#define odd10ca20e8c4c1d90cc4cfef8b0e13e

#include <cstdint>
#include <climits>
#include <cmath>

namespace ChowChow {
    class Frequency {
    public:
        using freq_t = uint64_t;

        static constexpr int FREQ_INT_BITS = 21; // enough for 20000 * 100
        static constexpr int FREQ_FRAC_BITS =
            sizeof(freq_t)*CHAR_BIT - FREQ_INT_BITS;
        static constexpr freq_t MAX_FRAC =
            (static_cast<uint64_t>(1) << FREQ_FRAC_BITS) - 1;
        static constexpr freq_t MAX_INT =
            (static_cast<uint64_t>(1) << FREQ_INT_BITS) - 1;

        Frequency() {};
        Frequency(freq_t n) :f{n} {};
        Frequency(double);
        Frequency(unsigned int);
        Frequency(int n) : Frequency(static_cast<unsigned int>(n)) {};

        freq_t intg() const { return f >> FREQ_FRAC_BITS; }
        freq_t frac() const { return f & MAX_FRAC; }
        freq_t raw() const { return f; }
        double make_double() const;

        Frequency& operator+=(Frequency fr) { f += fr.raw(); return *this; }
        Frequency& operator-=(Frequency fr) { f -= fr.raw(); return *this; }
        Frequency& operator*=(Frequency fr);

    private:
        freq_t f = 1;
    };

    inline Frequency operator+(Frequency a, Frequency b) { return a += b; }
    inline Frequency operator-(Frequency a, Frequency b) { return a -= b; }
    inline Frequency operator*(Frequency a, Frequency b) { return a *= b; }

    inline bool operator==(Frequency a, Frequency b)
    {
        return a.raw() == b.raw();
    }

    inline bool operator!=(Frequency a, Frequency b)
    {
        return a.raw() != b.raw();
    }

}

#endif
