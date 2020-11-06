#ifndef G668a9c31c3a4d2884c1e1b2f8a695a7
#define G668a9c31c3a4d2884c1e1b2f8a695a7

#include <cstdint>
#include <cmath>

namespace ChowChow {
    class Sample {
    public:
        using amp_t = unsigned short;
        using smp_t = short;
        static constexpr amp_t AMP_MAX = INT16_MIN * -1;
        static constexpr smp_t   SMP_MAX_POS = INT16_MAX;
        static constexpr int SMP_MAX_NEG = INT16_MIN * -1;

        constexpr Sample() {}
        constexpr Sample(smp_t n) :v{n} {}
        constexpr Sample(long double n)
        {
            if (std::signbit(n)) {
                v = static_cast<smp_t>(std::round(n * SMP_MAX_NEG));
            } else {
                v = static_cast<smp_t>(std::round(n * SMP_MAX_POS));
            }
        }
        constexpr Sample(double n) : Sample(static_cast<long double>(n)) {}
        constexpr Sample(int n) : Sample(static_cast<long double>(n)) {}
        constexpr Sample(long n) : Sample(static_cast<long double>(n)) {}
        constexpr Sample(long long n) : Sample(static_cast<long double>(n)) {}

        constexpr amp_t amp() const { return a; }
        constexpr long double amp_f() const
        {
            return static_cast<long double>(amp()) / AMP_MAX;
        }
        constexpr smp_t val() const { return v*amp() / AMP_MAX; }
        constexpr long double val_f() const
        {
            const auto v = static_cast<long double>(val());
            if (std::signbit(v)) {
                return v / SMP_MAX_NEG;
            } else {
                return v / SMP_MAX_POS;
            }
        }

        inline void amp(amp_t n) { a = n; }
        inline void val(Sample s) { v = s.val(); }

        void amp(long double n);
        void amp(double n);

    private:
        amp_t a = AMP_MAX;
        smp_t v = 0;
    };

    constexpr bool operator==(Sample a, Sample b) { return a.val() == b.val(); }
}

#endif
