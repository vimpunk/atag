# atag

Audio tag parsing C++ header-only library, supporting ID3v1, ID3v2, FLAC, APE and (soon) others

## Installation

atag consists purely of header files, so just place the contents of the `include` directory in your system-wide or your project's include folder.

## Work in progress

This library is still a work-in-progress, but it will eventually support most other audio tag formats as well as full audio tag editing for all of them. So for now only tag parsing is supported for the above listed tag types.

## Example

The following shows the basic usage:
```
if (atag::id3v2::is_tagged(source)) {
    atag::id3v2::tag tag = atag::id3v2::parse(source);
}
```
where `source` is some buffer type (`std::string`, `std::vector<char>`, `std::array<char, N>`, [`mio::mmap_source`](https://github.com/mandreyel/mio) or other).
All tag formats adhere to the above syntax, i.e.: `atag::{id3v1, id3v2, flac, ape}::{is_tagged, parse[, simple_parse]}`.

A simple ID3v2 or FLAC parser (since these two are the most popular) program to show the basic usage of atag:

```c++
#include <atag.hpp>

#include <fstream>
#include <sstream>
#include <cstdio>
#include <cstring>

int main(int argc, const char** argv)
{
    // Parse command line arguments.
    if (argc < 2) {
        std::printf("usage: %s, <path-to-music-file> [detailed]\n", argv[0]);
        return -1;
    }
    bool detailed = false;
    if (argc == 3 && std::strcmp(argv[2], "detailed") == 0) {
        detailed = true;
    }

    // Read in the file.
    const std::string source = [path = argv[1]] {
        std::ifstream file(path);
        if (!file) { throw std::runtime_error(std::string("could not open file ") + path); }
        std::stringstream ss;
        ss << file.rdbuf();
        return ss.str();
    }();

    using namespace atag;
    if (id3v2::is_tagged(source)) {
        std::printf("ID3v2 tag:\n");
        if (detailed) {
            // This will produce a lower level representation of the id3v2 tag.
            id3v2::tag tag = id3v2::parse(source);
            std::printf("tag:: version: %i, revision: %i, has_footer: %d, experimental: %d,"
                " has extended header: %d, unsynchronized: %d, #frames: %lu\n",
                tag.version, tag.revision, tag.flags & id3v2::tag::has_footer,
                tag.flags & id3v2::tag::experimental, tag.flags & id3v2::tag::extended,
                tag.flags & id3v2::tag::unsynchronisation, tag.frames.size());
            for (const auto& frame : tag.frames) {
                std::printf("%s : %s\n",
                    id3v2::frame_id_to_hrstring(frame.id),
                    frame.data.c_str());
            }
        } else {
            // While this produces a simpler tag with only a few key fields, such as
            // title, album, artist etc.
            simple_tag tag = id3v2::simple_parse(source);
            std::printf("title: %s\nalbum: %s\nartist: %s\nyear: %i\ntrack#: %i\n",
                tag.title.c_str(), tag.album.c_str(), tag.artist.c_str(), tag.year,
                tag.track_number);
        }
    } else if (flac::is_tagged(source)) {
        std::printf("FLAC tag:\n");
        flac::tag tag = flac::parse(source);
        std::printf("title: %s\nalbum: %s\nartist: %s\nyear: %i\ntrack#: %i\n"
            "sample rate: %i Hz\n#channels: %i\n#samples: %i\n",
            tag.title.c_str(), tag.album.c_str(), tag.artist.c_str(), tag.year,
            tag.track_number, tag.sample_rate, tag.num_channels, tag.num_samples);
    }
}
```
