#ifndef ATAG_ENCODING_HEADER
#define ATAG_ENCODING_HEADER

#include <string>
#include <codecvt>
#include <locale>

namespace atag {
namespace encoding {

std::string utf16le_to_utf8(const char16_t* s, const int size)
{
    static std::wstring_convert<std::codecvt_utf8_utf16<char16_t,
        0x10ffff, std::little_endian>, char16_t> convert;
    return convert.to_bytes(s, s + size);
}

std::string utf16be_to_utf8(const char16_t* s, const int size)
{
    static std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> convert;
    return convert.to_bytes(s, s + size);
}

std::u16string utf8_to_utf16le(const char* s, const int length)
{
    std::string source(s, length);
    static std::wstring_convert<std::codecvt_utf8_utf16<char16_t,
        0x10ffff, std::little_endian>, char16_t> convert;
    return convert.from_bytes(s, s + length);
}

std::u16string utf8_to_utf16be(const char* s, const int length)
{
    std::string source(s, length);
    static std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> convert;
    return convert.from_bytes(s, s + length);
}

} // namespace encoding
} // namespace atag

#endif // ATAG_ENCODING_HEADER
