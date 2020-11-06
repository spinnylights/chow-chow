#include<utility>
#include<bitset>
#include<sstream>
#include<fstream>
#include<climits>

#include "chow-chow/wav_file.hpp"

namespace fs = std::filesystem;
using namespace ChowChow;

WAVFile::WAVFile(const std::vector<long double>& samps,
                 unsigned int sample_rate)
    :ns{samps},
     sr{sample_rate}
{}

WAVFile::WAVFile(std::vector<long double>&& samps,
                 unsigned int sample_rate)
    :sr{sample_rate}
{
    std::swap(ns, samps);
}

std::string WAVFile::header() const
{
        const std::string CHUNK_ID = "RIFF";
        const std::string FORMAT = "WAVE";

        const std::string SUBCHUNK_1_ID = "fmt ";
        const std::bitset<32> SUBCHUNK_1_SIZE_BS = 16;
        const char* SUBCHUNK_1_SIZE =
                reinterpret_cast<const char*>(&SUBCHUNK_1_SIZE_BS);
        const std::bitset<16> AUDIO_FORMAT_BS = 1;
        const char* AUDIO_FORMAT =
                reinterpret_cast<const char*>(&AUDIO_FORMAT_BS);
        const std::bitset<16> NUM_CHANNELS_REP_BS = NUM_CHANNELS;
        const char* NUM_CHANNELS_REP =
                reinterpret_cast<const char*>(&NUM_CHANNELS_REP_BS);
        const std::bitset<32> sample_rate_REP_BS = sr;
        const char* sample_rate_REP =
                reinterpret_cast<const char*>(&sample_rate_REP_BS);
        const std::bitset<32> BYTE_RATE_BS =   sr
                                             * NUM_CHANNELS
                                             * (BITS_PER_SAMPLE/8);
        const char* BYTE_RATE =
                reinterpret_cast<const char*>(&BYTE_RATE_BS);
        const std::bitset<16> BLOCK_ALIGN_BS =   NUM_CHANNELS
                                               * (BITS_PER_SAMPLE/8);
        const char* BLOCK_ALIGN =
                reinterpret_cast<const char*>(&BLOCK_ALIGN_BS);
        const std::bitset<16> BITS_PER_SAMPLE_REP_BS = BITS_PER_SAMPLE;
        const char* BITS_PER_SAMPLE_REP =
                reinterpret_cast<const char*>(&BITS_PER_SAMPLE_REP_BS);

        const std::string SUBCHUNK_2_ID = "data";

        uint32_t subchunk_2_size = ns.size() * (BITS_PER_SAMPLE/8);
        std::bitset<32> chunk_size_bs = subchunk_2_size + 32;
        const char* chunk_size =
                reinterpret_cast<const char*>(&chunk_size_bs);
        std::bitset<32> subchunk_2_size_rep_bs = subchunk_2_size;
        const char* subchunk_2_size_rep =
                reinterpret_cast<const char*>(&subchunk_2_size_rep_bs);

        std::ostringstream os;
        os << CHUNK_ID;
        os.write(chunk_size, chunk_size_bs.size() / 8);
        os << FORMAT;
        os << SUBCHUNK_1_ID;
        os.write(SUBCHUNK_1_SIZE, SUBCHUNK_1_SIZE_BS.size() / 8);
        os.write(AUDIO_FORMAT, AUDIO_FORMAT_BS.size() / 8);
        os.write(NUM_CHANNELS_REP, NUM_CHANNELS_REP_BS.size() / 8);
        os.write(sample_rate_REP, sample_rate_REP_BS.size() / 8);
        os.write(BYTE_RATE, BYTE_RATE_BS.size() / 8);
        os.write(BLOCK_ALIGN, BLOCK_ALIGN_BS.size() / 8);
        os.write(BITS_PER_SAMPLE_REP, BITS_PER_SAMPLE_REP_BS.size() / 8);
        os << SUBCHUNK_2_ID;
        os.write(subchunk_2_size_rep, subchunk_2_size_rep_bs.size() / 8);
        return os.str();
}

std::string WAVFile::data() const
{
    std::stringstream s;

    for (auto const& n : ns) {
        std::bitset<BITS_PER_SAMPLE> n_sc =
            static_cast<int32_t>(n * 8388607);
        const int32_t n_ul = n_sc.to_ulong();
        const auto n_out = reinterpret_cast<const char*>(&n_ul);

        s.write(n_out, n_sc.size() / CHAR_BIT);
    }

    return s.str();
}

void WAVFile::write(std::filesystem::path path) const
{
    std::ofstream out {path, std::ios::binary};

    out << header();
    out << data();

    out.close();
}
