#include <mbgl/util/mapbox.hpp>
#include <mbgl/util/constants.hpp>
#include <mbgl/util/logging.hpp>
#include <mbgl/util/url.hpp>
#include <mbgl/util/tileset.hpp>

#include <stdexcept>
#include <vector>
#include <iostream>
#include <cstring>

namespace {

const char* protocol = "mapbox://";
const std::size_t protocolLength = 9;

} // namespace

namespace mbgl {
namespace util {
namespace mapbox {

bool isMapboxURL(const std::string& url) {
    return url.compare(0, protocolLength, protocol) == 0;
}

static bool equals(const std::string& str, const URL::Segment& segment, const char* ref) {
    return str.compare(segment.first, segment.second, ref) == 0;
}

std::string normalizeSourceURL(const std::string& baseURL,
                               const std::string& str,
                               const std::string& accessToken) {
    if (!isMapboxURL(str)) {
        return str;
    }
    if (accessToken.empty()) {
        throw std::runtime_error(
            "You must provide a Mapbox API access token for Mapbox tile sources");
    }

    const URL url(str);
    const auto tpl = baseURL + "/v4/{domain}.json?access_token=" + accessToken + "&secure";
    return transformURL(tpl, str, url);
}

std::string normalizeStyleURL(const std::string& baseURL,
                              const std::string& str,
                              const std::string& accessToken) {
    if (!isMapboxURL(str)) {
        return str;
    }

    const URL url(str);
    if (!equals(str, url.domain, "styles")) {
        Log::Error(Event::ParseStyle, "Invalid style URL");
        return str;
    }

    const auto tpl = baseURL + "/styles/v1{path}?access_token=" + accessToken;
    return transformURL(tpl, str, url);
}

std::string normalizeSpriteURL(const std::string& baseURL,
                               const std::string& str,
                               const std::string& accessToken) {
    if (!isMapboxURL(str)) {
        return str;
    }

    const URL url(str);
    if (!equals(str, url.domain, "sprites")) {
        Log::Error(Event::ParseStyle, "Invalid sprite URL");
        return str;
    }

    const auto tpl =
        baseURL + "/styles/v1{directory}{filename}/sprite{extension}?access_token=" + accessToken;
    return transformURL(tpl, str, url);
}

std::string normalizeGlyphsURL(const std::string& baseURL,
                               const std::string& str,
                               const std::string& accessToken) {
    if (!isMapboxURL(str)) {
        return str;
    }

    const URL url(str);
    if (!equals(str, url.domain, "fonts")) {
        Log::Error(Event::ParseStyle, "Invalid glyph URL");
        return str;
    }

    const auto tpl = baseURL + "/fonts/v1{path}?access_token=" + accessToken;
    return transformURL(tpl, str, url);
}

std::string normalizeTileURL(const std::string& baseURL,
                             const std::string& str,
                             const std::string& accessToken) {
    if (!isMapboxURL(str)) {
        return str;
    }

    const URL url(str);
    if (!equals(str, url.domain, "tiles")) {
        Log::Error(Event::ParseStyle, "Invalid tile URL");
        return str;
    }

    const auto tpl = baseURL + "/v4{path}?access_token=" + accessToken;
    return transformURL(tpl, str, url);
}

std::string
canonicalizeTileURL(const std::string& str, const style::SourceType type, const uint16_t tileSize) {
    const char* version = "/v4/";
    const size_t versionLen = strlen(version);

    const URL url(str);
    const Path path(str, url.path.first, url.path.second);

    // Make sure that we are dealing with a valid Mapbox tile URL.
    // Has to be /v4/, with a valid filename + extension
    if (str.compare(url.path.first, versionLen, version) != 0 || path.filename.second == 0 ||
        path.extension.second <= 1) {
        // Not a proper Mapbox tile URL.
        return str;
    }

    // Reassemble the canonical URL from the parts we've parsed before.
    std::string result = "mapbox://tiles/";
    result.append(str, path.directory.first + versionLen, path.directory.second - versionLen);
    result.append(str, path.filename.first, path.filename.second);
    if (type == style::SourceType::Raster || type == style::SourceType::RasterDEM) {
        result += tileSize == util::tileSize ? "@2x" : "{ratio}";
    }

#if !defined(__ANDROID__) && !defined(__APPLE__) && !defined(QT_IMAGE_DECODERS)
    const bool forceWebP = str.compare(path.extension.first, path.extension.second, ".png") == 0;
#else
    const bool forceWebP = false;
#endif // !defined(__ANDROID__) && !defined(__APPLE__) && !defined(QT_IMAGE_DECODERS)

    // Replace PNG with WebP if necessary.
    if (forceWebP) {
        result += ".webp";
    } else {
        result.append(str, path.extension.first, path.extension.second);
    }

    // Append the query string, minus the access token parameter.
    if (url.query.second > 1) {
        auto idx = url.query.first;
        bool hasQuery = false;
        while (idx != std::string::npos) {
            idx++; // skip & or ?
            auto ampersandIdx = str.find('&', idx);
            const char* accessToken = "access_token=";
            if (str.compare(idx, strlen(accessToken), accessToken) != 0) {
                result.append(1, hasQuery ? '&' : '?');
                result.append(str, idx, ampersandIdx != std::string::npos ? ampersandIdx - idx
                                                                          : std::string::npos);
                hasQuery = true;
            }
            idx = ampersandIdx;
        }
    }

    return result;
}

void canonicalizeTileset(Tileset& tileset, const std::string& sourceURL, style::SourceType type, uint16_t tileSize) {
    // TODO: Remove this hack by delivering proper URLs in the TileJSON to begin with.
    if (isMapboxURL(sourceURL)) {
        for (auto& url : tileset.tiles) {
            url = canonicalizeTileURL(url, type, tileSize);
        }
    }
}

const uint64_t DEFAULT_OFFLINE_TILE_COUNT_LIMIT = 6000;

} // end namespace mapbox
} // end namespace util
} // end namespace mbgl
