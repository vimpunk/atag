#ifndef ATAG_ID3V1_HEADER
#define ATAG_ID3V1_HEADER

#include <array>
#include <vector>
#include <string>
#include <initializer_list>

namespace atag {
namespace id3v1 {

struct tag
{

};

template<typename Source> tag parse(const Source& s);

} // namespace id3v1
} // namespace atag

//#include "impl/id3v1.ipp"

#endif // ATAG_ID3V1_HEADER
