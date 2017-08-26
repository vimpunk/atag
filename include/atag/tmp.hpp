/**
 * This is a direct representation of the tag found in an audio file.
 */
struct full_tag
{
    struct header
    {
        std::array<char, 3> magic;
        uint8_t version;
        uint8_t revision;
        uint8_t flags;
        uint32_t size;
    };

    struct extended_header
    {
        uint32_t size;
        uint8_t num_flag_bytes;
        uint16_t flags;
        std::string data;
    };

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
};



struct simple_tag
{
    std::string title;
    std::string album;
    std::string artist;
    int genre;
    int track_number;
    int length; // in ms
    int year;
};

/** Parses and extracts the most important frames in s. */
template<typename Source>
simple_tag simple_parse(const Source& s);

        {
            // this will produce the full id3v2 tag
            id3v2::full_tag full_tag = id3v2::full_parse(source);
            println("#frames: " << tag.frames.size());
            for(const auto& frame : tag.frames) { print_frame(frame); }
        }

        {
            // while this produce only a few key fields, such as title, album, artist etc
            id3v2::simple_tag tag = id3v2::simple_parse(source);
        }
