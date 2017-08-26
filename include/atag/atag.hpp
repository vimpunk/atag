#ifndef ATAG_HEADER
#define ATAG_HEADER

#include <vector>
#include <string>

#include "id3v1.hpp"
#include "id3v2.hpp"
#include "flac.hpp"
#include "ape.hpp"

namespace atag {

enum class genre
{

};

struct tag
{
    std::string title;
    std::string album;
    std::string artist;
    enum genre genre;
    int track_number;
    int length; // in ms
    int year;
};

/**
 * Parses an APE, ID3v1, or ID3v2 tagged music file and produces a generic tag with
 * the most important details about the music.
 */
template<typename Source> tag parse(const Source& s);

} // namespace atag

#include "impl/atag.ipp"

#endif // ATAG_HEADER
