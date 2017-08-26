#ifndef ATAG_IMPL_HEADER
#define ATAG_IMPL_HEADER

#include "../atag.hpp"

#include <algorithm>
#include <cstdlib>

namespace atag {

template<typename Source>
tag parse(const Source& s)
{
    tag t;
    // TODO
    if(id3v2::is_tagged(s))
    {
        using id3v2::tag;
        id3v2::tag::frame title{id3v2::title};
        id3v2::tag::frame album{id3v2::album};
        id3v2::tag::frame lead_artist{id3v2::lead_artist};
        id3v2::tag::frame year{id3v2::year};
        id3v2::tag::frame track_number{id3v2::track_number};
        id3v2::tag::frame length{id3v2::length};
        id3v2::tag::frame genre{id3v2::content_type};

        /*
        id3v2::parse(s, {album, title, year, track_number, lead_artist, length});

        t.title = std::move(title.data);
        t.album = std::move(album.data);
        t.artist = std::move(lead_artist.data);
        //t.genre = std::stoi(content_type.data);
        t.track_number = std::atoi(track_number.data.c_str());
        t.length = std::atoi(length.data.c_str());
        t.year = std::atoi(year.data.c_str());
        */
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
