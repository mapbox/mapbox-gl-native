#include <mbgl/util/utf.hpp>

#include <memory>
#include <locale>
#include <codecvt>

namespace mbgl {
namespace util {

std::u16string utf8_to_utf16::convert(std::string const& utf8) {
    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> converter;
    return converter.from_bytes(utf8);
}

std::string utf16_to_utf8::convert(std::u16string const& utf16) {
    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> converter;
    return converter.to_bytes(utf16);
}

    
} // namespace util
} // namespace mbgl
