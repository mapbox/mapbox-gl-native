#include <mbgl/text/default_font.hpp>

namespace mbgl {

static std::vector<std::string> defaultFontPaths;

void AddDefaultFontPath(const std::string& path) {
    defaultFontPaths.push_back(path);
}

std::vector<std::string> GetDefaultFontPaths() {
    return defaultFontPaths;
}

}
