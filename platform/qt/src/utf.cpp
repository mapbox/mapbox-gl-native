#include <mbgl/util/utf.hpp>

#include <QString>

namespace mbgl {
namespace util {

std::u16string utf8_to_utf16::convert(std::string const& utf8) {
    return QString::fromStdString(utf8).toStdU16String();
}

} // namespace util
} // namespace mbgl
