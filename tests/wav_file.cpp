#include <doctest.h>

#include <bitset>
#include <climits>
#include <cmath>

#include "chow-chow/wav_file.hpp"

using namespace ChowChow;

static const std::vector<long double> expected = {1., 0.5, -1., -0.5};

WAVFile file() { return WAVFile {expected, 48000}; }

TEST_CASE("length of header") {
    CHECK(file().header().length() == 44);
}

TEST_CASE("data conversion") {
    auto data = file().data();
    for (std::size_t i = 0; i < data.length(); i += WAVFile::BYTES_PER_SAMPLE) {
        std::bitset<WAVFile::BITS_PER_SAMPLE> sample = {};

        for (std::size_t j = 0; j < WAVFile::BITS_PER_SAMPLE / CHAR_BIT; ++j) {
            std::bitset<CHAR_BIT> bs = data[i+j];
            for (std::size_t k = 0; k < CHAR_BIT; ++k) {
                sample[k + CHAR_BIT*j] = bs[k];
            }
        }

        std::bitset<32> sample_n = {};
        for (std::size_t j = 0; j < sample.size() - 1; ++j) {
            sample_n[j] = sample[j];
        }
        sample_n[sample_n.size() - 1] = sample[sample.size() - 1];

        if (sample_n[sample_n.size() - 1] == 1) {
            for (std::size_t j = 0; j < 8; ++j) {
                sample_n[sample_n.size() - 9 + j] = 1;
            }
        }

        int32_t sample_i = sample_n.to_ulong();

        double sample_f =
            static_cast<double>(sample_i)
            / (std::pow(2, WAVFile::BITS_PER_SAMPLE - 1) - 1);

        CHECK(expected[i / WAVFile::BYTES_PER_SAMPLE] - sample_f < 1.e-7);
    }
}
