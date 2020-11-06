#include <doctest.h>

#include "chow-chow/wav_file.hpp"

using namespace ChowChow;

WAVFile file() { return WAVFile {{1., 0.5, -1., -0.5}, 48000}; }

TEST_CASE("length of header") {
    CHECK(file().header().length() == 44);
}

TEST_CASE("data conversion") {
    auto data = file().data();
    CHECK(data[0] == -1);
    CHECK(data[1] == 127);
    CHECK(data[2] == -1);
    CHECK(data[3] == 63);
    CHECK(data[4] == 1);
    CHECK(data[5] == -128);
    CHECK(data[6] == 1);
    CHECK(data[7] == -64);
}
