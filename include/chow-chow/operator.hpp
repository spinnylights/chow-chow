#ifndef S1ced45b2e3d4532b474c1fca1127b34
#define S1ced45b2e3d4532b474c1fca1127b34

#include <cstdint>
#include <vector>
#include <array>

#include "chow-chow/sample.hpp"
#include "chow-chow/setting.hpp"
#include "chow-chow/phase.hpp"

namespace ChowChow {
    class Operator {
    public:
        inline Operator(unsigned int sample_rate,
                        std::size_t _id = 0,
                        std::size_t op_count = 1)
            :sr{sample_rate},
             i{_id}
        {
            outputs = std::vector<Sample>(op_count);
            modulations = std::vector<Sample>(op_count);
        }

        static constexpr std::size_t OVERSAMPLING = 64;
        using overwin_t = std::array<long double, OVERSAMPLING>;

        static constexpr double SPREAD_MAX = 20.;
        static constexpr double PARTIALS_MAX = 50.;

        constexpr auto id() const { return i; }
        constexpr auto spread() const { return spr.val(); }
        constexpr auto spread_raw() const { return spr.val_raw(); }
        constexpr auto partials() const { return parts.val(); }
        constexpr auto partials_raw() const { return parts.val_raw(); }
        bool feedback() const;
        constexpr auto needs_recalc() const { return rc; }
        constexpr auto send_out() const { return send; }
        constexpr auto amp() const { return samp.amp(); }
        constexpr auto amp_f() const { return samp.amp_f(); }
        constexpr auto sample() const { return samp.val(); }
        constexpr auto sample_f() const { return samp.val_f(); }
        //constexpr auto position() const { return pos.phase_d(); }
        constexpr auto position() const { return pos; }
        constexpr auto o_step() const { return over_step; }

        inline void spread(double f) { spr.val(f); }
        inline void partials(double f) { parts.val(f); }
        inline void set_recalc(bool b) { rc = b; }
        void frequency(long double f);
        inline void advance() { pos += step; }
        inline void advance(Phase p) { pos += step + p; }
        inline void advance(unsigned int times)
        {
            for (unsigned int i = 0; i < times; ++i) { advance(); }
        }
        void set_output(std::size_t id, const Sample& amp);
        void set_mod_from(std::size_t id, const Sample& amp);
        //void calc();
        void calc(Operator mod, long double index);

    private:
        const std::size_t i;
        unsigned int sr;

        Setting spr {SPREAD_MAX};
        Setting parts = {PARTIALS_MAX};

        std::vector<Sample> outputs;
        std::vector<Sample> modulations;

        bool rc = false;
        bool send = false;

        Phase step;
        Phase over_step;
        Phase pos;

        Sample samp;
    };
}

#endif
