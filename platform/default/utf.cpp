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

} // namespace util
} // namespace mbgl
