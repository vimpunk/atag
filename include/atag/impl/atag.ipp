#ifndef ATAG_IMPL_HEADER
#define ATAG_IMPL_HEADER

#include "../atag.hpp"

#include <algorithm>
#include <cstdlib>

namespace atag {

template<typename Source>
simple_tag parse(const Source& s)
{
    simple_tag t;
    // TODO
    if(id3v2::is_tagged(s))
    {
        t = id3v2::simple_parse(s);
    }
    else if(flac::is_tagged(s))
    {
        flac::tag f = flac::parse(s);
        t.title = std::move(f.title);
        t.album = std::move(f.album);
        t.artist = std::move(f.artist);
        t.year = f.year;
        t.track_number = f.track_number;
    }
    /*
    if(id3v1::is_tagged(s))
    {
    }
    if(ape::is_tagged(s))
    {
    }
    */
    return t;
}

} // namespace atag

#endif // ATAG_IMPL_HEADER
