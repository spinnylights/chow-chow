#ifndef Q2c87974c4594de8864318a8ec5c855a
#define Q2c87974c4594de8864318a8ec5c855a

#include <cstdint>
#include <cmath>

#include "chow-chow/amp.hpp"

namespace ChowChow {
    class Phase {
    public:
        using phase_t = uint64_t;

        constexpr Phase() {}
        constexpr Phase(phase_t init) :ph{init} {}
        constexpr Phase(unsigned int init) :ph{init} {}
        constexpr Phase(unsigned short init) :ph{init} {}
        constexpr Phase(long init) :ph{static_cast<phase_t>(init)} {}
        constexpr Phase(int init) :ph{static_cast<phase_t>(init)} {}
        constexpr Phase(short init) :ph{static_cast<phase_t>(init)} {}
        constexpr Phase(long double init) { ph = std::round(init * PI); }
        constexpr Phase(double init) : Phase(static_cast<long double>(init)) {}

        //constexpr Phase(long double init)
        //{
        //    long double prelim = 0.;
        //    if (std::abs(init) > LONG_D_H_PI) {
        //        constexpr long double SCALE =
        //            static_cast<long double>(ZERO)/LONG_D_PI;
        //        prelim = std::ceil(init*SCALE);
        //    } else {
        //        if (std::signbit(init)) {
        //            constexpr long double SCALE =
        //                static_cast<long double>(ZERO+2)/LONG_D_PI;
        //            prelim = std::ceil(init*SCALE);
        //        } else {
        //            constexpr long double SCALE =
        //                static_cast<long double>(ZERO-3)/LONG_D_PI;
        //            prelim = std::ceil(init*SCALE);
        //        }
        //    }
        //    ph = static_cast<phase_t>(prelim + ZERO);
        //}

        //static constexpr long double LONG_SQRT_3 =
        //    1.7320508075688772935274463415058723669428052538103806280558L;
        static constexpr phase_t PI = UINT64_MAX;
        static constexpr phase_t ZERO = UINT64_MAX/2;
        static constexpr phase_t PI_N = 0;
        static constexpr phase_t PI_D2 = (PI/4)*3;
        static constexpr phase_t PI_ND2 = PI/4;
        static constexpr long double LONG_D_PI =
            3.14159265358979323846264338327950288419716939937510L;
        //static constexpr long double LONG_D_H_PI =
        //    1.570796326794896619256404479703093102216371335089206695556640625L;
        //static constexpr long double LONG_D_H_SQ_3_PI =
        //    2.7206990463513267758911173864632335984260993721391108633548L;

        constexpr auto phase() const { return ph; }
        constexpr long double phase_d() const
        {
            return static_cast<long double>(phase()) / PI;
        }
        constexpr long double phase_piscale() const
        {
            return (phase_d() - 0.5) * 2 * LONG_D_PI;
        }
        //constexpr long double phase_d() const
        //{
        //    if (phase() == ZERO) {
        //        return 0.0;
        //    } else {
        //        constexpr long double SCALE = LONG_D_PI/static_cast<long double>(ZERO);
        //        return static_cast<long double>(phase() -
        //                                        static_cast<long double>(ZERO))
        //            * SCALE;
        //    }
        //}
        constexpr Amp amp() const
        {
            if (phase() == ZERO) {
                return 0;
            } else {
                constexpr Amp SCALE {340612158008655459};
                Amp a {(Amp{phase()} - Amp {ZERO}) * SCALE};
                if (a > 0) {
                    if (a > (Amp::PI*6/8) && a < (Amp::PI*7/8)) { a += 5; }
                    if (a > Amp::PI/2) { a -= 1; }
                    if (a < Amp::PI/2) { a += 1; }
                } else if (a <= (-Amp::PI*7/8) && a > (-Amp::PI*8/9)) { a += 1; }
                return a;
            }
        }

        void phase(phase_t pha) { ph = pha; };

        Phase& operator+=(Phase w) { phase(phase() + w.phase()); return *this; }
        Phase& operator-=(Phase w) { phase(phase() - w.phase()); return *this; }

    private:
        phase_t ph = ZERO;
    };

    inline Phase operator+(Phase a, Phase b) { return a += b; }
    inline Phase operator-(Phase a, Phase b) { return a -= b; }

    inline bool operator==(Phase a, Phase b) { return a.phase() == b.phase(); }
    inline bool operator!=(Phase a, Phase b) { return a.phase() != b.phase(); }
}

#endif
