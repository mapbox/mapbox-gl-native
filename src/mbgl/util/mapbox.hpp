#ifndef MBGL_UTIL_MAPBOX
#define MBGL_UTIL_MAPBOX

#include <string>
#include <mbgl/style/types.hpp>

namespace mbgl {
namespace util {
namespace mapbox {

bool isMapboxURL(const std::string& url);

std::string normalizeSourceURL(const std::string& url, const std::string& accessToken);
std::string normalizeStyleURL(const std::string& url, const std::string& accessToken);
std::string normalizeSpriteURL(const std::string& url, const std::string& accessToken);
std::string normalizeGlyphsURL(const std::string& url, const std::string& accessToken);
std::string normalizeTileURL(const std::string& url, const std::string& accessToken);

// Return a "mapbox://tiles/..." URL (suitable for normalizeTileURL) for the given Mapbox tile URL.
std::string canonicalizeTileURL(const std::string& url, SourceType, uint16_t tileSize);

} // namespace mapbox
} // namespace util
} // namespace mbgl

#endif
