#ifndef MBGL_UTIL_UTF
#define MBGL_UTIL_UTF

#include <memory>

// g++/libstdc++ is missing c++11 codecvt support
#if 1//! defined(__clang__) || defined(__linux__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-local-typedefs"
//#include <boost/locale.hpp>
#include <boost/regex/pending/unicode_iterator.hpp>
#pragma GCC diagnostic pop
#else
// Assume that codecvt is present on clang on non-linux systems
#include <codecvt>
#include <locale>
#endif

namespace mbgl {

namespace util {

#if 1 //! defined(__clang__) || defined(__linux__)

class utf8_to_utf32 {
 public:
    explicit utf8_to_utf32() {}
    std::u32string convert(std::string const& utf8)
    {
        boost::u8_to_u32_iterator<std::string::const_iterator> begin(utf8.begin());
        boost::u8_to_u32_iterator<std::string::const_iterator> end(utf8.end());
        return std::u32string(begin,end);
    }
};

#else

class utf8_to_utf32 {
 public:
    explicit utf8_to_utf32()
        : utf32conv_() {}
    std::u32string convert(std::string const& utf8) {
        return utf32conv_.from_bytes(utf8);
    }
 private:
    std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> utf32conv_;
};

#endif

}}

#endif
