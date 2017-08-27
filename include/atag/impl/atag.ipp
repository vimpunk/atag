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
