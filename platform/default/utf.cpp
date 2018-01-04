#include <mbgl/util/utf.hpp>

#include <memory>
#include <locale>

// GCC 4.9 compatibility
#if !defined(__GNUC__) || __GNUC__ >= 5
#include <codecvt>
#else
#include <boost/locale/encoding_utf.hpp>
#endif

namespace mbgl {
namespace util {

std::u16string utf8_to_utf16::convert(std::string const& utf8) {
#if !defined(__GNUC__) || __GNUC__ >= 5
    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> converter;
    return converter.from_bytes(utf8);
#else
    return boost::locale::conv::utf_to_utf<char16_t>(utf8.c_str(), utf8.c_str() + utf8.size());
#endif
}

} // namespace util
} // namespace mbgl
