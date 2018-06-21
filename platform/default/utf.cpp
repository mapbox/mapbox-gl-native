#include <mbgl/util/utf.hpp>

#include <locale>
#include <codecvt>

namespace mbgl {
namespace util {

std::u16string utf8_to_utf16::convert(const std::string& utf8) {
    return std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t>().from_bytes(utf8);
}

} // namespace util
} // namespace mbgl
