#ifndef e40829217ed44cd589e5c4edbbdba590
#define e40829217ed44cd589e5c4edbbdba590

#include<vector>
#include<filesystem>
#include<string>
#include<climits>

namespace ChowChow {
    /**
     * @brief Represents audio in RIFF WAVE format.
     *
     * Represents audio as a 24-bit stereo RIFF WAVE file.
     * Supports writing it to disk for convenience.
     */
    class WAVFile {
    public:
        static constexpr uint32_t NUM_CHANNELS = 2;
        static constexpr uint32_t BITS_PER_SAMPLE = 24;
        static constexpr uint32_t BYTES_PER_SAMPLE = BITS_PER_SAMPLE / CHAR_BIT;

        /**
         * @param samples a stereo-interleaved collection of
         * samples.
         *
         * @param sample_rate in Hz.
         */
        WAVFile(const std::vector<double>& samples,
                unsigned int sample_rate);

        /**
         * @copydoc WAVFile(const std::vector<double>&,
         * unsigned int)
         */
        WAVFile(std::vector<double>&& samples,
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
        const unsigned int sr;

        std::vector<double> ns = {};
    };
}

#endif
