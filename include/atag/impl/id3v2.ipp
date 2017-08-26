#ifndef ATAG_ID3_IMPL_HEADER
#define ATAG_ID3_IMPL_HEADER

#include "../id3v2.hpp"
#include "../detail/type_traits.hpp"

#include <cassert>
#include <algorithm>
#include <iterator>
#include <array>
#ifdef ATAG_ENABLE_DEBUGGING
# include <cstdio>
# define ATAG_BYTE_BINARY_PATTERN "%c%c%c%c %c%c%c%c"
# define ATAG_BYTE_TO_BINARY(byte) \
    (byte & 0x80 ? '1' : '0'), \
    (byte & 0x40 ? '1' : '0'), \
    (byte & 0x20 ? '1' : '0'), \
    (byte & 0x10 ? '1' : '0'), \
    (byte & 0x08 ? '1' : '0'), \
    (byte & 0x04 ? '1' : '0'), \
    (byte & 0x02 ? '1' : '0'), \
    (byte & 0x01 ? '1' : '0')
#endif // ATAG_ENABLE_DEBUGGING

namespace atag {
namespace id3v2 {

// Both are endianness agnostic.
template<typename InputIt>
uint32_t parse_syncsafe_int(InputIt it)
{
    uint32_t t = 0;
    t |= static_cast<uint32_t>(static_cast<uint8_t>(*it++)) << 3 * 7;
    t |= static_cast<uint32_t>(static_cast<uint8_t>(*it++)) << 2 * 7;
    t |= static_cast<uint32_t>(static_cast<uint8_t>(*it++)) <<     7;
    t |= static_cast<uint32_t>(static_cast<uint8_t>(*it++));
    return t;
}

template<typename OutputIt>
void write_syncsafe_int(uint32_t t, OutputIt it)
{
    assert(0 && "TODO");
}

// -- data --

struct tag_header
{
    enum flags : uint8_t
    {
        has_footer        = 1 << 4,
        experimental      = 1 << 5,
        extended          = 1 << 6,
        unsynchronisation = 1 << 7,
    };

    std::array<uint8_t, 3> magic;
    uint8_t version;
    uint8_t revision;
    uint8_t flags;
    uint32_t size;
    uint32_t extended_header_size;
};

/*
struct extended_header
{
    uint32_t size;
    // Points to actual flags in the input buffer.
    string_view flags;
};
*/

struct frame_header
{
    enum flags : uint16_t
    {
        // -- frame status flags --
        tag_preservation  = 1 << 14,
        file_preservation = 1 << 13,
        read_only         = 1 << 12,
        // -- frame format flags --
        grouping_identity = 1 << 6,
        compression       = 1 << 3,
        encryption        = 1 << 2,
        unsynchronisation = 1 << 1,
        length_indicator  = 1 << 0,
    };

    int id;
    uint32_t size;
    uint16_t flags;
};

// -- functions --

template<typename String>
tag_header parse_tag_header(const String& s) noexcept
{
    tag_header h;
    std::copy(&s[0], &s[3], &h.magic[0]);
    h.version = s[3];
    h.revision = s[4];
    h.flags = s[5];
    h.size = parse_syncsafe_int(&s[6]);
#ifdef ATAG_ENABLE_DEBUGGING
    std::printf("tag:: magic: %s, version: %i, revision: %i, size: %i, flags: "
        ATAG_BYTE_BINARY_PATTERN"\n", h.magic.data(), h.version, h.revision, h.size,
        ATAG_BYTE_TO_BINARY(h.flags));
#endif // ATAG_ENABLE_DEBUGGING
    if(h.flags & tag_header::flags::extended)
        h.extended_header_size = parse_syncsafe_int(&s[10]);
    else
        h.extended_header_size = 0;
    return h;
}

/*
template<typename String>
inline extended_header parse_extended_header(const String& s) noexcept
{
    extended_header h;
    h.size = parse_syncsafe_int(&s[10]);
    const int num_flag_bytes = s[11];
    h.flags = string_view(&s[12], num_flag_bytes);
    return h;
}
*/

template<typename String>
frame_header parse_frame_header(const String& s) noexcept
{
    frame_header h;
    h.id = frame_id_from_string(s);
    h.size = parse_syncsafe_int(&s[4]);
    // TODO is this correct? (regarding endianness and such)
    std::copy(&s[8], &s[10], &h.flags);
    return h;
}

template<typename String>
tag::frame parse_frame_body(const frame_header& header, const String& data)
{
    // TODO FIXME we'll need a lot more branching here depending on the frame's type
    tag::frame frame;
    frame.id = header.id;
    if(is_text_frame(header.id) || (frame.id == comments))
    {
        // textual information
        frame.encoding = data[0];
        frame.data = std::string(&data[1], &data[header.size]); // +1 to skip encoding
    }
    else
    {
        frame.data = std::string(&data[0], &data[header.size]);
    }
    return frame;
}

inline bool is_tag_header_valid(const tag_header& header) noexcept
{
    // TODO
    return true;
}

inline bool is_frame_header_valid(const frame_header& header) noexcept
{
    // TODO
    return header.id != -1;
}

template<typename Source>
inline int find_tag_start(const Source& s) noexcept
{
    const auto matches_at = [&s](const char* magic, const int p)
    {
        return std::equal(&s[p], &s[p+3], magic)
            && (s[p+3] <= 4) && (s[p+4] < 0xff)
            && (s[p+6] < 0x80) && (s[p+7] < 0x80) && (s[p+8] < 0x80) && (s[p+9] < 0x80);
    };

    // most ID3v2 tags will be prepended to the file, so start with that
    if(matches_at("ID3", 0)) { return 0; }

    // see if the tag is appended (in which case it must have a footer at the very end
    // of the file)
    const int n = s.size();
    if(matches_at("3DI", n - 10))
    {
        const auto tag_size = parse_syncsafe_int(&s[n-4]);
        // since there is a header and a footer, subtract the 10 byte header size twice
        return n - tag_size - 20;
    }

    // no tag could be found
    return -1;
}

template<typename Source>
bool is_tagged(const Source& s) noexcept
{
    // FIXME doesn't work properly
    //static_assert(detail::is_source<Source>::value, "Source requirements not met");
    return find_tag_start(s) != -1;
}

template<typename Source>
tag parse(const Source& s)
{
    return parse(s, [](const int _) { return true; });
}

template<typename Source>
tag parse(const Source& s, const std::initializer_list<int>& wanted_frames)
{
    return parse(s, [&wanted_frames](const int id)
        { return std::find(wanted_frames.begin(), wanted_frames.end(), id)
                    != wanted_frames.end(); });
}

template<typename Source, typename Predicate>
tag parse(const Source& s, Predicate pred)
{
    //static_assert(detail::is_source<Source>::value, "Source requirements not met");

    if(s.size() < 10) { throw "source must be at least 10 bytes long"; }

    const int tag_start = find_tag_start(s);
    const auto tag_header = parse_tag_header(&s[tag_start]);
    if(!is_tag_header_valid(tag_header)) { return {}; }

    tag tag;
    // now parse the frames, starting after the header + extended header
    for(auto i = tag_start + 10 + tag_header.extended_header_size; i < tag_header.size;)
    {
        const auto frame_header = parse_frame_header(&s[i]);
        if(is_frame_header_valid(frame_header) && pred(frame_header.id))
        {
            tag.frames.emplace_back(parse_frame_body(frame_header, &s[i+10]));
#ifdef ATAG_ENABLE_DEBUGGING
            std::printf("parsed frame:: id: %s(%s), size: %i, flags: "
                ATAG_BYTE_BINARY_PATTERN " " ATAG_BYTE_BINARY_PATTERN", data: %s\n",
                frame_id_to_hrstring(frame_header.id),
                frame_id_to_string(frame_header.id), frame_header.size,
                ATAG_BYTE_TO_BINARY(uint8_t(frame_header.flags >> 8)),
                ATAG_BYTE_TO_BINARY(uint8_t(frame_header.flags)),
                tag.frames.back().data.c_str());
#endif // ATAG_ENABLE_DEBUGGING
        }
        i += frame_header.size + 10;
    }
    return tag;
}

// NOTE: this _must_ be kept in the same order as the tag::frame::id values.
constexpr static const struct {
    int id;
    const char* raw;
    const char* human_readable;
} frame_ids[] = {
    {tag::frame::aenc, "AENC", "audio encryption"},
    {tag::frame::apic, "APIC", "attached picture"},
    {tag::frame::aspi, "ASPI", "audio seek point index"},
    {tag::frame::comm, "COMM", "comments"},
    {tag::frame::comr, "COMR", "commercial frame"},
    {tag::frame::encr, "ENCR", "encryption method registration"},
    {tag::frame::equa, "EQUA", "equalisation"},
    {tag::frame::etco, "ETCO", "event timing codes"},
    {tag::frame::geob, "GEOB", "general encapsulated object"},
    {tag::frame::grid, "GRID", "group identification registration"},
    {tag::frame::ipls, "IPLS", "involved people list"},
    {tag::frame::link, "LINK", "linked information"},
    {tag::frame::mcdi, "MCDI", "music CD identifier"},
    {tag::frame::mllt, "MLLT", "MPEG location lookup table"},
    {tag::frame::owne, "OWNE", "ownership frame"},
    {tag::frame::pcnt, "PCNT", "play counter"},
    {tag::frame::popm, "POPM", "popularimeter"},
    {tag::frame::poss, "POSS", "position synchronisation frame"},
    {tag::frame::priv, "PRIV", "private frame"},
    {tag::frame::rbuf, "RBUF", "recommended buffer size"},
    {tag::frame::rvad, "RVAD", "relative volume adjustment"},
    {tag::frame::rvrb, "RVRB", "reverb"},
    {tag::frame::seek, "SEEK", "seek frame"},
    {tag::frame::sign, "SIGN", "signature frame"},
    {tag::frame::sylt, "SYLT", "synchronised lyric/text"},
    {tag::frame::sytc, "SYTC", "synchronized tempo codes"},

    {tag::frame::talb, "TALB", "album"},
    {tag::frame::tbpm, "TBPM", "beats/minute"},
    {tag::frame::tcom, "TCOM", "composer"},
    {tag::frame::tcon, "TCON", "content type"},
    {tag::frame::tcop, "TCOP", "copyright"},
    {tag::frame::tdat, "TDAT", "date"},
    {tag::frame::tden, "TDEN", "encoding time"},
    {tag::frame::tdly, "TDLY", "playlist delay"},
    {tag::frame::tenc, "TENC", "encoded by"},
    {tag::frame::text, "TEXT", "lyricist(s)"},
    {tag::frame::tflt, "TFLT", "file type"},
    {tag::frame::time, "TIME", "time"},
    {tag::frame::tit1, "TIT1", "content group"},
    {tag::frame::tit2, "TIT2", "title"},
    {tag::frame::tit3, "TIT3", "subtitle"},
    {tag::frame::tkey, "TKEY", "initial key"},
    {tag::frame::tlan, "TLAN", "language(s)"},
    {tag::frame::tlen, "TLEN", "length (ms)"}, 
    {tag::frame::tmed, "TMED", "media type"},
    {tag::frame::toal, "TOAL", "original title"},
    {tag::frame::tofn, "TOFN", "original filename"},
    {tag::frame::toly, "TOLY", "original lyricist(s)"},
    {tag::frame::tope, "TOPE", "original performer"},
    {tag::frame::tory, "TORY", "original release year"},
    {tag::frame::town, "TOWN", "licensee"},
    {tag::frame::tpe1, "TPE1", "lead performer(s)"},
    {tag::frame::tpe2, "TPE2", "band/orchestra/accompaniment"},
    {tag::frame::tpe3, "TPE3", "conductor/performer"},
    {tag::frame::tpe4, "TPE4", "interpreted/remixed/modified by"},
    {tag::frame::tpos, "TPOS", "part of a set"},
    {tag::frame::tpub, "TPUB", "publisher"},
    {tag::frame::trck, "TRCK", "track number"},
    {tag::frame::trda, "TRDA", "recording dates"},
    {tag::frame::trsn, "TRSN", "internet radio station name"},
    {tag::frame::trso, "TRSO", "internet radio station owner"},
    {tag::frame::tsiz, "TSIZ", "size (bytes)"},
    {tag::frame::tsrc, "TSRC", "ISRC"},
    {tag::frame::tsse, "TSSE", "encoding software/hardware settings"},
    {tag::frame::txxx, "TXXX", "user defined text"},
    {tag::frame::tyer, "TYER", "year"},

    {tag::frame::ufid, "UFID", "unique file identifier"},
    {tag::frame::user, "USER", "terms of use"},
    {tag::frame::uslt, "USLT", "unsynchronized lyrics"},

    {tag::frame::wcom, "WCOM", "commercial information"},
    {tag::frame::wcop, "WCOP", "copyright information"},
    {tag::frame::woaf, "WOAF", "offical audio file webpage"},
    {tag::frame::woar, "WOAR", "offical artist webpage"},
    {tag::frame::woas, "WOAS", "official audio source webpage"},
    {tag::frame::wors, "WORS", "official internet radio station homepage"},
    {tag::frame::wpay, "WPAY", "payment"},
    {tag::frame::wpub, "WPUB", "publisher official webpage"},
    {tag::frame::wxxx, "WXXX", "user defined link"}
};

template<typename Source>
int frame_id_from_string(const Source& s) noexcept
{
    const auto it = std::find_if(std::begin(frame_ids), std::end(frame_ids),
        [&s](const auto& f) { return std::equal(f.raw, f.raw + 4, &s[0]); });
    if(it != std::end(frame_ids))
        return it->id;
    else
        return -1;
}

constexpr const char* frame_id_to_string(const int id) noexcept
{
    if((id >= int(tag::frame::aenc)) && (id <= int(tag::frame::wxxx)))
        return frame_ids[id].raw; 
    else
        return nullptr;
}

constexpr const char* frame_id_to_hrstring(const int id) noexcept
{
    if((id >= int(tag::frame::aenc)) && (id <= int(tag::frame::wxxx)))
        return frame_ids[id].human_readable; 
    else
        return nullptr;
}

} // namespace id3v2
} // namespace atag

#endif // ATAG_ID3_IMPL_HEADER
