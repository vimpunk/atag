#ifndef ATAG_APE_HEADER
#define ATAG_APE_HEADER

namespace atag {
namespace ape {

struct tag
{

};

template<typename Source> tag parse(const Source& s);

} // namespace ape
} // namespace atag

#endif // ATAG_APE_HEADER
