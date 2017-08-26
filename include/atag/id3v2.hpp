#ifndef ATAG_ID3V2_HEADER
#define ATAG_ID3V2_HEADER

#include <vector>
#include <string>
#include <initializer_list>

namespace atag {
namespace id3v2 {

enum encoding
{
    iso_8859_1 = 0,
    utf16 = 1,
    utf16be = 2,
    utf8 = 3,
};

// TODO maybe provide two ways of parsing:
// 1) parse the entire raw tag, as is, with every field in the header, extended header
// and all frames (or just the ones specified by user
// 2) a higher level alternative that extracts only the usual fields, such as the title,
// album, artist, year etc
// because right now it's sort of in between: a user can neither parse the full raw
// tag (not sure how useful it is, as some of the info in the "raw tag" is just for
// parsing), but they still have to loop through tags and find whatever they're looking
// for...

struct tag
{
    struct frame
    {
        /**
         * So as to remain unambiguous with the official ID3 documentation, the original 
         * cryptic frame names have been preserved. To see what an individual frame
         * means, see: http://id3.org/id3v2.4.0-frames or use id3v2::hrid, which maps
         * a more readable alternative to each entry here.
         */
        enum id
        {
            aenc, apic, aspi, comm, comr, encr, equa, etco, geob, grid, ipls, link, mcdi,
            mllt, owne, pcnt, popm, poss, priv, rbuf, rvad, rvrb, seek, sign, sylt, sytc,

            // -- text information frames --
            talb, tbpm, tcom, tcon, tcop, tdat, tden, tdly, tenc, text, tflt, time, tit1,
            tit2, tit3, tkey, tlan, tlen, tmed, toal, tofn, toly, tope, tory, town, tpe1,
            tpe2, tpe3, tpe4, tpos, tpub, trck, trda, trsn, trso, tsiz, tsrc, tsse, txxx,
            tyer,

            ufid, user, uslt,

            // -- URL link frames --
            wcom, wcop, woaf, woar, woas, wors, wpay, wpub, wxxx,
        };

        // Identifies what type of frame this is.
        uint8_t id;
        // The encoding of data if it's a text, irrelevant otherwise.
        uint8_t encoding;
        std::string data;
    };

    std::vector<frame> frames;
};

inline bool is_text_frame(const int id) noexcept
{
    return (id >= tag::frame::talb) && (id <= tag::frame::tyer);
}

inline bool is_url_frame(const int id) noexcept
{
    return (id >= tag::frame::wcom) && (id <= tag::frame::wxxx);
}

/**
 * Returns -1 if s does not contain a valid frame id. s must be at least 4 bytes long,
 * otherwise undefined behaviour may ensue.
 *
 * s has to be at least 4 bytes long.
 */
template<typename String>
int frame_id_from_string(const String& s) noexcept;

/** Both return a nullptr if id is not a valid frame id. */
constexpr const char* frame_id_to_string(const int id) noexcept;
constexpr const char* frame_id_to_hrstring(const int id) noexcept;

/** Tests whether s contains an ID3v2 tag, which may be prepended or appended. */
template<typename Source>
bool is_tagged(const Source& s) noexcept;

/** Parses and extracts all frames found in s. */
template<typename Source>
tag parse(const Source& s);

/**
 * This overload provides a way to parse only the frames specified in wanted_frames.
 *
 * Example:
 *
 * using namespace atag;
 * id3v2::tag tag = id3v2::parse(source, {id3v2::composer, id3v2::album,
 *     id3v2::title, id3v2::year, id3v2::track_number};
 * for(const auto& frame : tag.frames) {
 *     // do something with frame
 * }
 */
template<typename Source>
tag parse(const Source& s, const std::initializer_list<int>& wanted_frames);

/**
 *This overload expects a user defined comparator which should take a single int or
 * tag::frame::id argument and return true if the frame satisfies the user's criteria.
 *
 * Example:
 *
 * using namespace atag;
 * // parse all text information frames
 * auto tag1 = id3v2::parse(source1, id3v2::is_text_frame);
 * // or:
 * auto tag2 = id3v2::parse(source2, [](const int frame_id) { // ...  });
 */
template<typename Source, typename Predicate>
tag parse(const Source& s, Predicate pred);

enum hrid
{
    audio_encryption = tag::frame::aenc,
    attached_picture = tag::frame::apic,
    audio_seek_point_index = tag::frame::aspi,
    comments = tag::frame::comm,
    commercial_frame = tag::frame::comr,
    encryption_method_registration = tag::frame::encr,
    equalisation = tag::frame::equa,
    event_timing_codes = tag::frame::etco,
    general_encapsulated_object = tag::frame::geob,
    group_identification_registration = tag::frame::grid,
    involved_people_list = tag::frame::ipls,
    linked_information = tag::frame::link,
    music_cd_id = tag::frame::mcdi,
    mpeg_location_lookup_table = tag::frame::mllt,
    ownership_frame = tag::frame::owne,
    play_counter = tag::frame::pcnt,
    popularimeter = tag::frame::popm,
    position_sync_frame = tag::frame::poss,
    private_frame = tag::frame::priv,
    recommended_buffer_size = tag::frame::rbuf,
    relative_vol_adjustment = tag::frame::rvad,
    reverb = tag::frame::rvrb,
    seek_frame = tag::frame::seek,
    signature_frame = tag::frame::sign,
    synced_lyryc = tag::frame::sylt,
    synced_tempo_codes = tag::frame::sytc,

    album = tag::frame::talb,
    beats_per_min = tag::frame::tbpm,
    composer = tag::frame::tcom,
    content_type = tag::frame::tcon,
    copyright = tag::frame::tcop,
    date = tag::frame::tdat,
    encoding_time = tag::frame::tden,
    playlist_delay = tag::frame::tdly,
    encoded_by = tag::frame::tenc,
    lyricist = tag::frame::text,
    file_type = tag::frame::tflt,
    time = tag::frame::time,
    content_group = tag::frame::tit1,
    title = tag::frame::tit2,
    subtitle = tag::frame::tit3,
    initial_key = tag::frame::tkey,
    language = tag::frame::tlan,
    length = tag::frame::tlen,
    media_type = tag::frame::tmed,
    original_title = tag::frame::toal,
    original_filename = tag::frame::tofn,
    original_lyricist = tag::frame::toly,
    original_performer = tag::frame::tope,
    original_release_year = tag::frame::tory,
    licensee = tag::frame::town,
    lead_artist = tag::frame::tpe1,
    accompaniment = tag::frame::tpe2,
    conductor = tag::frame::tpe3,
    remixed_by = tag::frame::tpe4,
    part_of_a_set = tag::frame::tpos,
    publisher = tag::frame::tpub,
    track_number = tag::frame::trck,
    recording_dates = tag::frame::trda,
    inet_radio_station_name = tag::frame::trsn,
    inet_radio_station_owner = tag::frame::trso,
    size = tag::frame::tsiz,
    isrc = tag::frame::tsrc,
    encoding_settings = tag::frame::tsse,
    user_defined_text = tag::frame::txxx,
    year = tag::frame::tyer,

    unique_file_id = tag::frame::ufid,
    terms_of_use = tag::frame::user,
    unsynced_lyrics = tag::frame::uslt,

    commercial_info = tag::frame::wcom,
    copyright_info = tag::frame::wcop,
    audio_file_webpage = tag::frame::woaf,
    artist_webpage = tag::frame::woar,
    audio_source_webpage = tag::frame::woas,
    internet_radio_station_webpage = tag::frame::wors,
    payment = tag::frame::wpay,
    publisher_webpage = tag::frame::wpub,
    user_defined_link = tag::frame::wxxx,
};

} // namespace id3v2
} // namespace atag

#include "impl/id3v2.ipp"

#endif // ATAG_ID3V2_HEADER
