#ifndef MBGL_UTIL_UTF
#define MBGL_UTIL_UTF

#include <memory>

#include <boost/regex/pending/unicode_iterator.hpp>

namespace mbgl {

namespace util {

class utf8_to_utf32 {
 public:
    static std::u32string convert(std::string const& utf8)
    {
        boost::u8_to_u32_iterator<std::string::const_iterator> begin(utf8.begin());
        boost::u8_to_u32_iterator<std::string::const_iterator> end(utf8.end());
        return std::u32string(begin,end);
    }
};

}}

#endif
