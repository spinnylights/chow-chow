#ifndef y7a0528fdc7c4764acdb295ffd032bb0
#define y7a0528fdc7c4764acdb295ffd032bb0

#include <cstdint>
#include <climits>
#include <cmath>
#include <string>

namespace ChowChow {
    class Amp {
    public:
        // Aliases
        using amp_t = __int128;
        using amp_ut = unsigned __int128;
        using amp_down_t = int64_t;

        // Constants
        static const int AMP_SIZE = sizeof(amp_t)*CHAR_BIT;
        static const amp_t AMP_MAX =
            static_cast<amp_t>((static_cast<amp_ut>(1) << (AMP_SIZE - 1)) - 1);
        static const amp_t AMP_MIN = -AMP_MAX - 1;
        static const amp_down_t AMP_DOWN_MAX =
            static_cast<amp_down_t>(static_cast<amp_ut>(AMP_MAX) >> AMP_SIZE/2);
        static constexpr amp_down_t AMP_DOWN_MAX_P10 = []{
            amp_down_t max = AMP_DOWN_MAX/10;
            amp_down_t tens = 1;

            while (max) {
                max /= 10;
                tens *= 10;
            }

            return tens;
        }();

        static constexpr amp_t SCALE_DOWN =
            static_cast<amp_t>(AMP_DOWN_MAX_P10);
        static constexpr int DECIMALS = 18;
        static constexpr amp_t PI = 3141592653589793238;

        // Methods
        constexpr Amp() {}
        constexpr Amp(amp_t n) :a{n} {}
        constexpr Amp(long long n) :a{n} {}
        constexpr Amp(long n) :a{n} {}
        constexpr Amp(int n) :a{n} {}
        constexpr Amp(short n) :a{n} {}
        constexpr Amp(unsigned long long n) :a{n} {}
        constexpr Amp(unsigned long n) :a{n} {}
        constexpr Amp(unsigned int n) :a{n} {}
        constexpr Amp(unsigned short n) :a{n} {}
        constexpr Amp(long double n)
            :a {static_cast<amp_t>(n * std::pow(10, DECIMALS))} {}
        constexpr Amp(double n) : Amp(static_cast<long double>(n)) {}

        constexpr auto amp() const { return a; }

        constexpr void amp(const Amp& m) { a = m.amp(); }

        std::string to_s() const
        {
            if (amp() <= AMP_DOWN_MAX) {
                return std::to_string(static_cast<amp_down_t>(amp()));
            } else {
                const amp_t top = amp() / AMP_DOWN_MAX_P10;
                const amp_down_t bottom =
                    static_cast<amp_down_t>(amp() % AMP_DOWN_MAX_P10);

                return std::string {Amp {top}.to_s() + std::to_string(bottom)};
            }
        }

        // Operators
        constexpr Amp& operator+=(const Amp& m)
        {
            amp(amp() + m.amp()); return *this;
        }

        constexpr Amp& operator-=(const Amp& m)
        {
            amp(amp() - m.amp()); return *this;
        }

        constexpr Amp& operator*=(const Amp& m)
        {
            amp((amp() * m.amp()) / SCALE_DOWN); return *this;
        }

    private:
        amp_t a = 0;
    };

    constexpr Amp operator+(Amp a, const Amp& b) { return a += b; }
    constexpr Amp operator-(Amp a, const Amp& b) { return a -= b; }
    constexpr Amp operator*(Amp a, const Amp& b) { return a *= b; }

    constexpr bool operator==(const Amp& a, const Amp& b)
    {
        return a.amp() == b.amp();
    }

    constexpr bool operator!=(const Amp& a, const Amp& b)
    {
        return a.amp() != b.amp();
    }

    constexpr bool operator>=(const Amp& a, const Amp& b)
    {
        return a.amp() >= b.amp();
    }

    constexpr bool operator>(const Amp& a, const Amp& b)
    {
        return a.amp() > b.amp();
    }

    constexpr bool operator<=(const Amp& a, const Amp& b)
    {
        return a.amp() <= b.amp();
    }

    constexpr bool operator<(const Amp& a, const Amp& b)
    {
        return a.amp() < b.amp();
    }
}

#endif
