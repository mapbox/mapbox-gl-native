#include <mbgl/util/platform.hpp>

#include <QByteArray>
#include <QString>

#include <string>

namespace mbgl {
namespace platform {

std::string uppercase(const std::string& str) {
    auto upper = QString::fromUtf8(str.data(), str.length()).toUpper().toUtf8();

    return std::string(upper.constData(), upper.size());
}

std::string lowercase(const std::string& str) {
    auto lower = QString::fromUtf8(str.data(), str.length()).toLower().toUtf8();

    return std::string(lower.constData(), lower.size());
}

} // namespace platform
} // namespace mbgl
