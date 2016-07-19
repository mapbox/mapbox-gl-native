#include <mbgl/util/mapbox.hpp>
#include <mbgl/util/constants.hpp>
#include <mbgl/platform/log.hpp>

#include <stdexcept>
#include <vector>
#include <iostream>

namespace mbgl {
namespace util {
namespace mapbox {

const std::string protocol = "mapbox://";
const std::string baseURL = "https://api.mapbox.com/";

bool isMapboxURL(const std::string& url) {
    return std::equal(protocol.begin(), protocol.end(), url.begin());
}

std::vector<std::string> getMapboxURLPathname(const std::string& url) {
    std::vector<std::string> pathname;
    auto startIndex = protocol.length();
    auto end = url.find_first_of("?#");
    if (end == std::string::npos) {
        end = url.length();
    }
    while (startIndex < end) {
        auto endIndex = url.find("/", startIndex);
        if (endIndex == std::string::npos) {
            endIndex = end;
        }
        pathname.push_back(url.substr(startIndex, endIndex - startIndex));
        startIndex = endIndex + 1;
    }
    return pathname;
}

std::string normalizeSourceURL(const std::string& url, const std::string& accessToken) {
    if (!isMapboxURL(url)) {
        return url;
    }

    if (accessToken.empty()) {
        throw std::runtime_error("You must provide a Mapbox API access token for Mapbox tile sources");
    }

    return baseURL + "v4/" + url.substr(protocol.length()) + ".json?access_token=" + accessToken + "&secure";
}

std::string normalizeStyleURL(const std::string& url, const std::string& accessToken) {
    if (!isMapboxURL(url)) {
        return url;
    }

    const auto pathname = getMapboxURLPathname(url);
    if (pathname.size() < 3) {
        Log::Error(Event::ParseStyle, "Invalid style URL");
        return url;
    }

    const auto& user = pathname[1];
    const auto& id = pathname[2];
    const bool isDraft = pathname.size() > 3;
    return baseURL + "styles/v1/" + user + "/" + id + (isDraft ? "/draft" : "") + "?access_token=" + accessToken;
}

std::string normalizeSpriteURL(const std::string& url, const std::string& accessToken) {
    if (!isMapboxURL(url)) {
        return url;
    }

    const auto pathname = getMapboxURLPathname(url);
    if (pathname.size() < 3) {
        Log::Error(Event::ParseStyle, "Invalid sprite URL");
        return url;
    }

    const auto& user = pathname[1];
    const bool isDraft = pathname.size() > 3;

    const auto& name = isDraft ? pathname[3] : pathname[2];
    const size_t index = name.find_first_of("@.");
    if (index == std::string::npos) {
        Log::Error(Event::ParseStyle, "Invalid sprite URL");
        return url;
    }
    const auto& extension = name.substr(index);

    if (isDraft) {
        const auto& id = pathname[2];
        return baseURL + "styles/v1/" + user + "/" + id + "/draft/sprite" + extension +
               "?access_token=" + accessToken;

    } else {
        const auto& id = pathname[2].substr(0, index);
        return baseURL + "styles/v1/" + user + "/" + id + "/sprite" + extension + "?access_token=" +
               accessToken;
    }
}

std::string normalizeGlyphsURL(const std::string& url, const std::string& accessToken) {
    if (!isMapboxURL(url)) {
        return url;
    }

    const auto pathname = getMapboxURLPathname(url);
    if (pathname.size() < 4) {
      Log::Error(Event::ParseStyle, "Invalid glyph URL");
      return url;
    }

    const auto& user = pathname[1];
    const auto& fontstack = pathname[2];
    const auto& range = pathname[3];

    return baseURL + "fonts/v1/" + user + "/" + fontstack + "/" + range + "?access_token=" + accessToken;
}

std::string normalizeTileURL(const std::string& url, const std::string& accessToken) {
    if (!isMapboxURL(url)) {
        return url;
    }

    return baseURL + "v4/" + url.substr(sizeof("mapbox://tiles/") - 1) + "?access_token=" + accessToken;
}

std::string canonicalizeTileURL(const std::string& url, SourceType type, uint16_t tileSize) {
    auto tilesetStartIdx = url.find("/v4/");
    if (tilesetStartIdx == std::string::npos) {
        return url;
    }

    tilesetStartIdx += sizeof("/v4/") - 1;

    const auto tilesetEndIdx = url.find("/", tilesetStartIdx);
    if (tilesetEndIdx == std::string::npos) {
        return url;
    }

    auto queryIdx = url.rfind("?");
    if (queryIdx == std::string::npos) {
        queryIdx = url.length();
    }

    auto basenameIdx = url.rfind("/", queryIdx);
    if (basenameIdx == std::string::npos || basenameIdx == queryIdx - 1) {
        basenameIdx = url.length();
    } else {
        basenameIdx += 1;
    }

    const auto extensionIdx = url.find(".", basenameIdx);
    if (extensionIdx == std::string::npos || extensionIdx == queryIdx - 1) {
        return url;
    }

    const auto tileset = url.substr(tilesetStartIdx, tilesetEndIdx - tilesetStartIdx);
    auto extension = url.substr(extensionIdx + 1, queryIdx - extensionIdx - 1);

#if !defined(__ANDROID__) && !defined(__APPLE__) && !defined(QT_IMAGE_DECODERS)
    // Replace PNG with WebP.
    if (extension == "png") {
        extension = "webp";
    }
#endif // !defined(__ANDROID__) && !defined(__APPLE__) && !defined(QT_IMAGE_DECODERS)

    std::string result = "mapbox://tiles/" + tileset + "/{z}/{x}/{y}";

    if (type == SourceType::Raster) {
        result += tileSize == util::tileSize ? "@2x" : "{ratio}";
    }

    result += "." + extension;
    return result;
}

const uint64_t DEFAULT_OFFLINE_TILE_COUNT_LIMIT = 6000;

} // end namespace mapbox
} // end namespace util
} // end namespace mbgl
