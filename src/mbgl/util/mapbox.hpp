#pragma once

#include <string>
#include <mbgl/style/types.hpp>

namespace mbgl {

class Tileset;

namespace util {
namespace mapbox {

bool isMapboxURL(const std::string& url);

std::string normalizeSourceURL(const std::string& baseURL, const std::string& url, const std::string& accessToken);
std::string normalizeStyleURL(const std::string& baseURL, const std::string& url, const std::string& accessToken);
std::string normalizeSpriteURL(const std::string& baseURL, const std::string& url, const std::string& accessToken);
std::string normalizeGlyphsURL(const std::string& baseURL, const std::string& url, const std::string& accessToken);
std::string normalizeTileURL(const std::string& baseURL, const std::string& url, const std::string& accessToken);

// Return a "mapbox://tiles/..." URL (suitable for normalizeTileURL) for the given Mapbox tile URL.
std::string canonicalizeTileURL(const std::string& url, style::SourceType, uint16_t tileSize);

// Replace URL templates with "mapbox://tiles/..." URLs (suitable for normalizeTileURL).
void canonicalizeTileset(Tileset&, const std::string& url, style::SourceType, uint16_t tileSize);

extern const uint64_t DEFAULT_OFFLINE_TILE_COUNT_LIMIT;

} // namespace mapbox
} // namespace util
} // namespace mbgl
