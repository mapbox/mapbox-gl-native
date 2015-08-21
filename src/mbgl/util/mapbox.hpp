#ifndef MBGL_UTIL_MAPBOX
#define MBGL_UTIL_MAPBOX

#include <string>
#include <mbgl/style/types.hpp>

namespace mbgl {
namespace util {
namespace mapbox {

std::string normalizeSourceURL(const std::string& url, const std::string& accessToken);
std::string normalizeStyleURL(const std::string& url, const std::string& accessToken);
std::string normalizeGlyphsURL(const std::string& url, const std::string& accessToken);
std::string normalizeTileURL(const std::string& url, const std::string& sourceURL, SourceType sourceType);

}
}
}

#endif
