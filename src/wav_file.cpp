#include<utility>
#include<bitset>
#include<sstream>
#include<fstream>
#include<climits>

#include "chow-chow/wav_file.hpp"

namespace fs = std::filesystem;
using namespace ChowChow;

WAVFile::WAVFile(const std::vector<double>& samps,
                 unsigned int sample_rate)
    :ns{samps},
     sr{sample_rate}
{}

WAVFile::WAVFile(std::vector<double>&& samps,
                 unsigned int sample_rate)
    :sr{sample_rate}
{
    std::swap(ns, samps);
}

std::string WAVFile::header() const
{
    static constexpr int PCM = 16;
    static constexpr int LINEAR_QUANT = 1;

    static const std::string CHUNK_ID = "RIFF";
    static const std::string FORMAT = "WAVE";

    static const std::string SUBCHUNK_1_ID = "fmt ";
    static constexpr std::bitset<32> SUBCHUNK_1_SIZE_BS = PCM;
    static const char* SUBCHUNK_1_SIZE =
            reinterpret_cast<const char*>(&SUBCHUNK_1_SIZE_BS);
    static constexpr std::bitset<16> AUDIO_FORMAT_BS = LINEAR_QUANT;
    static const char* AUDIO_FORMAT =
            reinterpret_cast<const char*>(&AUDIO_FORMAT_BS);
    static constexpr std::bitset<16> NUM_CHANNELS_REP_BS = NUM_CHANNELS;
    static const char* NUM_CHANNELS_REP =
            reinterpret_cast<const char*>(&NUM_CHANNELS_REP_BS);
    static const std::bitset<32> sample_rate_REP_BS = sr;
    static const char* sample_rate_REP =
            reinterpret_cast<const char*>(&sample_rate_REP_BS);
    static const std::bitset<32> BYTE_RATE_BS =
        sr * NUM_CHANNELS * (BITS_PER_SAMPLE/8);
    static const char* BYTE_RATE =
            reinterpret_cast<const char*>(&BYTE_RATE_BS);
    static const std::bitset<16> BLOCK_ALIGN_BS =
        NUM_CHANNELS * (BITS_PER_SAMPLE/8);
    static const char* BLOCK_ALIGN =
            reinterpret_cast<const char*>(&BLOCK_ALIGN_BS);
    static const std::bitset<16> BITS_PER_SAMPLE_REP_BS = BITS_PER_SAMPLE;
    static const char* BITS_PER_SAMPLE_REP =
            reinterpret_cast<const char*>(&BITS_PER_SAMPLE_REP_BS);

    static const std::string SUBCHUNK_2_ID = "data";

    uint32_t subchunk_2_size = ns.size() * (BITS_PER_SAMPLE/8);
    std::bitset<32> chunk_size_bs = subchunk_2_size + 32;
    static const char* chunk_size =
            reinterpret_cast<const char*>(&chunk_size_bs);
    std::bitset<32> subchunk_2_size_rep_bs = subchunk_2_size;
    static const char* subchunk_2_size_rep =
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
