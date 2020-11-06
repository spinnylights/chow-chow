#include <numeric>
#include <algorithm>
#include <iostream>

#include "chow-chow/operator.hpp"

using namespace ChowChow;

bool Operator::feedback() const
{
    bool fb = false;

    for (std::size_t i = 0; i < outputs.size(); ++i) {
        if (outputs.at(i).val() > 0 && i >= id()) {
            fb = true;
            break;
        }
    }

    return fb;
}

void Operator::set_output(const std::size_t i,
                                    const Sample& amp)
{
    outputs.at(i).val(amp.val());
}

void Operator::set_mod_from(const std::size_t i,
                                      const Sample& amp)
{
    modulations.at(i).val(amp.val());
}

void Operator::frequency(long double f)
{
    const auto rate = f/sr;
    step = Phase {rate};
    over_step = Phase {rate/OVERSAMPLING};
}

/* In my environment (x86-64 Linux 5.9.1, Intel i7-7700K CPU) and
 * compiled with GCC 10.2.0, this function runs around 95% faster
 * than std::sin() for long doubles (according to callgrind) and
 * approximates sine over [-pi, pi] to 11 decimals at worst
 * (accuracy measured in SageMath with 128-bit floats). In the
 * case of regular doubles, it runs around 65% faster than
 * std::sin.
 *
 * With only 11 decimals of accuracy, it may be a bit silly to
 * use long doubles here, but since they're used for extra
 * precision elsewhere in the library and this function is
 * considerably more accurate in places it may not be worth
 * throwing away the extra information from the long double
 * input. Testing will tell for sure.
 */
constexpr long double sin_appr(long double x)
{
    constexpr long double h {-0.99999999998633767378040992292469063427L};
    constexpr long double g {0.16666666651020180412927128086551236896L};
    constexpr long double f {-0.0083333330343979108793639294698190945941L};
    constexpr long double e {0.00019841247787297215126917290768756413811L};
    constexpr long double d {-2.7556510906737132903478113302758095794e-6L};
    constexpr long double c {2.5035844683583533429417583696017346282e-8L};
    constexpr long double b {-1.5875580753992403046083004704558741255e-10L};
    constexpr long double a {6.5496701178847003743832538335854997571e-13L};

    const auto x2  = x*x;
    const auto x3  = x*x2;
    const auto x5  = x3*x2;
    const auto x7  = x5*x2;
    const auto x9  = x7*x2;
    const auto x11 = x9*x2;
    const auto x13 = x11*x2;
    const auto x15 = x13*x2;

    return a*x15 + b*x13 + c*x11 + d*x9 + e*x7 + f*x5 + g*x3 + h*x;
}

// assumes sr of 48000*32; would be better to calculate at
// init based on sample rate + oversampling
class ButterLP {
public:
    void filter(Operator::overwin_t& over_win)
    {
        with(cs1, over_win);
        with(cs2, over_win);
        with(cs3, over_win);
    }

private:
    long double zm1 = 0.;
    long double zm2 = 0.;

    static constexpr long double half_p = 10.;
    static constexpr long double t_pi_d_sr = 4.0906154343617099459149e-6;
    //const long double b = 2.0L - std::cos((half_p * t_pi_d_sr));
    const long double b = 1.00000085673637;
    const long double c2 = b - std::sqrt(b*b - 1.0L);
    const long double c1 = 1.0L - c2;
    long double q = 0.0;
    long double r = 0.0;

    using full_cs_t = std::array<long double, 5>;
    using short_cs_t = std::array<long double, 3>;
    static constexpr full_cs_t cs1 =
        {0.00000001851674619, 0.00000003703349239, 0.00000001851674619,
         -1.80374577991888652, 0.81399570855984904};
    static constexpr short_cs_t cs2 =
        {2., -1.84984360477753018, 0.86035548796916439};
    static constexpr short_cs_t cs3 =
        {2., -1.93552045630701142, 0.94651920501158882};

    void reset_zs(Operator::overwin_t& over_win)
    {
        constexpr auto win_zm1 = Operator::OVERSAMPLING - 1;
        constexpr auto win_zm2 = win_zm1 - 1;

        zm1 = over_win[win_zm1];
        zm2 = over_win[win_zm2];
    }

    void with(const full_cs_t& cs, Operator::overwin_t& over_win)
    {
        //reset_zs(over_win);

        for (auto&& i : over_win) {
            //auto A = i * cs[0];
            auto A = i;
            A -= cs[3] * zm1;
            A -= cs[4] * zm2;
            const auto z = A;
            A += cs[1] * zm1 + cs[2]*zm2;

            //q = c1 * A * A + c2 * q;
            //r = c1 * i * i + c2 * r;
            q = A * A + q;
            r = i * i + r;
            if (q != 0.0) {
              A *= std::sqrt(r/q);
            } else {
              A *= std::sqrt(r);
            }

            i = A;
            zm2 = zm1;
            zm1 = z;
        }
    }

    void with(const short_cs_t& cs, Operator::overwin_t& over_win)
    {
        //reset_zs(over_win);

        for (auto&& i : over_win) {
            auto A = i;
            A -= cs[1] * zm1;
            A -= cs[2] * zm2;
            const auto z = A;
            A += cs[0] * zm1 + zm2;

            //q = c1 * A * A + c2 * q;
            //r = c1 * i * i + c2 * r;
            q = A * A + q;
            r = i * i + r;
            if (q != 0.0) {
              A *= sqrt(r/q);
            } else {
              A *= sqrt(r);
            }

            i = A;
            zm2 = zm1;
            zm1 = z;
        }
    }
};

//void Operator::calc()
void Operator::calc(Operator mod, long double index)
{
    constexpr long double PI = Phase::LONG_D_PI;
    constexpr long double TAU = PI * 2.L;
    constexpr long double RADIAN_SCALE = 1.L / TAU;

    auto over_pos = pos;
    auto mod_pos = mod.position();

    overwin_t over_win;
    for (auto&& s : over_win) {
        //const auto mod_s = sin_appr(mod_pos.phase_piscale()) * index * RADIAN_SCALE;
        const auto mod_s = std::sin(mod_pos.phase_piscale()) * index * RADIAN_SCALE;
        auto pre_scale = mod_s + over_pos.phase_piscale();
        while (pre_scale > PI) { pre_scale -= TAU; }
        while (pre_scale < -PI) { pre_scale += TAU; }
        //s = sin_appr(pre_scale);
        s = std::sin(pre_scale);
        over_pos += over_step;
        mod_pos += mod.o_step();
    }

    //const long double pre_vol =
    //    std::abs(*std::max_element(over_win.begin(),
    //                               over_win.end()));
    //ButterLP filt {};
    //filt.filter(over_win);

    //const long double post_vol =
    //    std::abs(*std::max_element(over_win.begin(),
    //                               over_win.end()));

    //const long double diff = [&]
    //{
    //    if (post_vol == 0.) {
    //        return 1.L;
    //    } else {
    //        return pre_vol / post_vol;
    //    }
    //}();

    //std::cout << "pre: " << pre_vol << "\n";
    //std::cout << "post: " << post_vol << "\n";
    //std::cout << "diff: " << diff << "\n";

    samp.val((std::accumulate(over_win.begin(),
                             over_win.end(),
                             0.) / OVERSAMPLING) * 0.9);
}
