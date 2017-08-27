#include "../include/atag/atag.hpp"
#include "../include/atag/detail/io_util.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <cassert>

#define println(m) do std::cout << m << '\n'; while(0)

void print_frame(const atag::id3v2::tag::frame& frame)
{
    // textual information
    println(atag::id3v2::frame_id_to_hrstring(frame.id) << ": " << frame.data);
}

template<typename String>
std::string read_file_data(const String& path)
{
    std::ifstream file(path);
    std::stringstream ss;
    ss << file.rdbuf();
    return ss.str();
}

int main(int argc, const char** argv)
{
    char src[4] = {0,0,0b1,0b0111'1111};
    assert(atag::detail::parse_syncsafe_int(src) == 255);
    assert(atag::detail::parse_syncsafe<int>(src) == 255);

    const std::string source = read_file_data(argc > 1 ? argv[1] : "sample.mp3");

    using namespace atag;
    if(id3v2::is_tagged(source))
    {
        println("file has id3v2 tag!");

        {
            // this will produce the a lower representation of the id3v2 tag
            id3v2::tag tag = id3v2::full_parse(source);
            std::printf("tag:: version: %i, revision: %i, has_footer: %d, experimental: %d,"
                " has extended header: %d, unsynchronized: %d, #frames: %i\n",
                tag.version, tag.revision, tag.flags & id3v2::tag::has_footer,
                tag.flags & id3v2::tag::experimental, tag.flags & id3v2::tag::extended,
                tag.flags & id3v2::tag::unsynchronisation, tag.frames.size());
            for(const auto& frame : tag.frames) { print_frame(frame); }
        }

        {
            // while this produces only a few key fields, such as title, album, artist etc
            simple_tag tag = id3v2::simple_parse(source);
            std::printf("title: %s, album: %s, artist: %s, year: %i, track#: %i\n",
                tag.title.c_str(), tag.album.c_str(), tag.artist.c_str(), tag.year,
                tag.track_number);
        }
    }
    else if(flac::is_tagged(source))
    {
        println("file has FLAC tag!");

        flac::tag tag = atag::flac::parse(source);
        std::printf("title: %s, album: %s, artist: %s, year: %i, track#: %i,"
            " sample rate: %i Hz, #channels: %i, #samples: %i\n",
            tag.title.c_str(), tag.album.c_str(), tag.artist.c_str(), tag.date,
            tag.track_number, tag.sample_rate, tag.num_channels, tag.num_samples);
    }
}
