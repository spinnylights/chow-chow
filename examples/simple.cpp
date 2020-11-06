#include <vector>

#include "chow-chow/operator.hpp"
#include "chow-chow/wav_file.hpp"

std::vector<long double> make_envelope(size_t len)
{
        std::vector<long double> env;
        const uint32_t ramp_time = len/50;
        for (size_t i = 0; i < ramp_time; ++i) {
                env.push_back((long double)i / ramp_time);
        }
        for (size_t i = 0; i < len - 2*ramp_time; ++i) {
                env.push_back(1.L);
        }
        for (size_t i = 0; i < ramp_time; ++i) {
                env.push_back(1 - ((long double)i / ramp_time));
        }
        return env;
}

std::vector<long double> make_upramp(size_t len)
{
        std::vector<long double> env;
        for (size_t i = 0; i < len; ++i) {
                env.push_back((long double)i / len);
        }
        return env;
}

int main(void)
{
    constexpr int sr = 48000;
    constexpr size_t len = sr*16;
    constexpr long double freq = 200.;
    constexpr long double ratio = 19.4;
    constexpr long double index = 500;

    ChowChow::Operator op {sr};
    op.frequency(freq);

    ChowChow::Operator mod {sr};
    mod.frequency(freq*ratio);

    std::vector<long double> s {len};

    const auto env = make_envelope(len/2);
    const auto ramp = make_upramp(len/2);

    for (size_t i = 0; i < len/2; ++i) {
        op.calc(mod, index * ramp.at(i));
        const auto samp = op.sample_f() * env.at(i);
        s.push_back(samp);
        s.push_back(samp);
        op.advance();
        mod.advance();
    }

    ChowChow::WAVFile file {s, sr};

    file.write("./simple.wav");

    return 0;
}
