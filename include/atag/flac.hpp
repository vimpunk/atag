#ifndef ATAG_FLAC_HEADER
#define ATAG_FLAC_HEADER

#include <vector>
#include <string>
#include <initializer_list>

namespace atag {
namespace flac {

enum encoding
{
    iso_8859_1 = 0,
    utf16 = 1,
    utf16be = 2,
    utf8 = 3,
};

struct tag
{
    std::string title;
    std::string album;
    std::string artist;
    std::string genre;
    int date;
    int track_number;
    int track_total;
    int sample_rate; // in Hz
    int num_channels;
    int total_samples;
};

/** Tests whether s contains a FLAC tag. */
template<typename Source>
bool is_tagged(const Source& s) noexcept;

/** Parses and extracts all frames found in s. */
template<typename Source>
tag parse(const Source& s);

} // namespace flac
} // namespace atag

//#include "impl/flac.ipp"

#endif // ATAG_FLAC_HEADER
