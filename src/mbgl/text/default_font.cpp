#include <mbgl/text/default_font.hpp>

namespace mbgl {

static std::string defaultFontPath;

void SetDefaultFontPath(const std::string& path) {
    defaultFontPath = path;
}

std::string GetDefaultFontPath() {
    return defaultFontPath;
}

}
