#ifndef e40829217ed44cd589e5c4edbbdba590
#define e40829217ed44cd589e5c4edbbdba590

#include<vector>
#include<filesystem>
#include<string>

#include "operator.hpp"

namespace ChowChow {
    /**
     * @brief Represents audio in RIFF WAVE format.
     *
     * Represents audio as a 24-bit stereo RIFF WAVE file.
     * Supports writing it to disk for convenience.
     */
    class WAVFile {
    public:
        /**
         * @param samples a stereo-interleaved collection of
         * samples.
         *
         * @param sample_rate in kHz.
         */
        WAVFile(const std::vector<long double>& samples,
                unsigned int sample_rate);

        /**
         * @copydoc WAVFile(const std::vector<long double>&,
         * unsigned int)
         */
        WAVFile(std::vector<long double>&& samples,
                unsigned int sample_rate);

        /**
         * @brief The RIFF WAVE header.
         */
        std::string header() const;

        /**
         * @brief The formatted sound data.
         */
        std::string data() const;

        /**
         * @brief Writes the whole WAVE file to disk.
         *
         * @param path the location to write to (including the
         * filename).
         */
        void write(std::filesystem::path path) const;

    private:
        static constexpr uint32_t NUM_CHANNELS = 2;
        static constexpr uint32_t BITS_PER_SAMPLE = 24;

        const unsigned int sr;

        std::vector<long double> ns = {};
    };
}

#endif
