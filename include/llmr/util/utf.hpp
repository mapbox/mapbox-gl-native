#ifndef LLMR_UTIL_UTF
#define LLMR_UTIL_UTF

#include <memory>

// g++/libstdc++ is missing c++11 codecvt support
#ifdef __linux__
#pragma GCC diagnostic push
#ifndef __clang__
#pragma GCC diagnostic ignored "-Wunused-local-typedefs"
#endif
#include <boost/locale.hpp>
#pragma GCC diagnostic pop
#else
#include <codecvt>
#include <locale>
#endif

namespace llmr {

namespace util {

#ifdef __linux__

class utf8_to_utf32 {
 public:
    explicit utf8_to_utf32() {}
    std::u32string convert(std::string const& utf8) {
        return boost::locale::conv::utf_to_utf<char32_t>(utf8);
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
