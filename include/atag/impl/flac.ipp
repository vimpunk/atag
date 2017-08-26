#ifndef ATAG_FLAC_IMPL_HEADER
#define ATAG_FLAC_IMPL_HEADER

#include "../flac.hpp"

#include <algorithm>
#include <array>

namespace atag {
namespace flac {

struct metadata_block
{
    enum type
    {
        streaminfo,
        padding,
        application,
        seektable,
        vorbis_comment,
        cuesheet,
        picture
    };

    struct header
    {
        uint32_t is_last_block :1;
        uint32_t block_type    :7;
        uint32_t length        :24;
    };

    enum
    {
        min_block_size = 16,
        max_block_size = 65535
    };

    struct header header;
};

struct streaminfo_block
{
    struct metadata_block::header header;
    uint16_t min_block_size;
    uint16_t max_block_size;
    int min_frame_size;
    int max_frame_size;
    int sample_rate;
    int8_t num_channels;
    int8_t bits_per_sample;
    uint64_t total_samples;
    std::array<uint8_t, 128> md5_sig;
};

/** Tests whether s contains a FLAC tag. */
template<typename Source>
bool is_tagged(const Source& s) noexcept
{
    //static_assert(detail::is_source<Source>::value, "Source requirements not met");
    return s.size() >= 5 // TODO verify
        && std::equal(&s[0], &s[4], "fLaC");
        // ignore the most significant bit as that's for the 'is_last_block'
        //&& (s[5] & 0b0111'1111)
}

/** Parses and extracts all frames found in s. */
template<typename Source>
tag parse(const Source& s)
{
    //static_assert(detail::is_source<Source>::value, "Source requirements not met");

    if(!is_tagged(s)) { return {}; }

    tag t;
    for(auto i = 0; !(s[i] & 0b1000'0000);)
    {
        const uint8_t block_type = s[i] & 0b0111'1111;
        switch(block_type)
        {
        case metadata_block::type::streaminfo:
        case metadata_block::type::padding:
        case metadata_block::type::application:
        case metadata_block::type::seektable:
        case metadata_block::type::vorbis_comment:
        case metadata_block::type::cuesheet:
        case metadata_block::type::picture:
        default: break;
        }
    }
    return t;
}

} // namespace flac
} // namespace atag

#endif // ATAG_FLAC_IMPL_HEADER
