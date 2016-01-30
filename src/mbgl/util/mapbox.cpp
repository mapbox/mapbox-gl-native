#include <mbgl/util/mapbox.hpp>
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
    std::size_t startIndex = protocol.length();
    while (startIndex < url.length()) {
        std::size_t endIndex = url.find("/", startIndex);
        if (endIndex == std::string::npos) {
            endIndex = url.find_first_of("?#");
        }
        if (endIndex == std::string::npos) {
            endIndex = url.length();
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

    std::vector<std::string> pathname = getMapboxURLPathname(url);

    if (pathname.size() < 3) {
        Log::Error(Event::ParseStyle, "Invalid style URL");
        return url;
    }

    std::string user = pathname[1];
    std::string id = pathname[2];
    bool isDraft = pathname.size() > 3;
    return baseURL + "styles/v1/" + user + "/" + id + (isDraft ? "/draft" : "") + "?access_token=" + accessToken;
}

std::string normalizeSpriteURL(const std::string& url, const std::string& accessToken) {
    if (!isMapboxURL(url)) {
        return url;
    }

    std::vector<std::string> pathname = getMapboxURLPathname(url);

    if (pathname.size() < 3) {
        Log::Error(Event::ParseStyle, "Invalid sprite URL");
        return url;
    }

    std::string user = pathname[1];
    bool isDraft = pathname.size() > 3;

    std::string id, extension;
    if (isDraft) {
        size_t index = pathname[3].find_first_of("@.");
        id = pathname[2];
        extension = pathname[3].substr(index);
    } else {
        size_t index = pathname[2].find_first_of("@.");
        id = pathname[2].substr(0, index);
        extension = pathname[2].substr(index);
    }

    return baseURL + "styles/v1/" + user + "/" + id + "/" + (isDraft ? "draft/" : "") + "sprite" + extension + "?access_token=" + accessToken;
}

std::string normalizeGlyphsURL(const std::string& url, const std::string& accessToken) {
    if (!isMapboxURL(url)) {
        return url;
    }

    std::vector<std::string> pathname = getMapboxURLPathname(url);

    if (pathname.size() < 4) {
      Log::Error(Event::ParseStyle, "Invalid glyph URL");
      return url;
    }

    std::string user = pathname[1];
    std::string fontstack = pathname[2];
    std::string range = pathname[3];

    return baseURL + "fonts/v1/" + user + "/" + fontstack + "/" + range + "?access_token=" + accessToken;
}

std::string normalizeTileURL(const std::string& url, const std::string& accessToken) {
    if (!isMapboxURL(url)) {
        return url;
    }

    return baseURL + "v4/" + url.substr(sizeof("mapbox://tiles/") - 1) + "?access_token=" + accessToken;
}

std::string canonicalizeTileURL(const std::string& url, SourceType type) {
    auto tilesetStartIdx = url.find("/v4/");
    if (tilesetStartIdx == std::string::npos) {
        return url;
    }

    tilesetStartIdx += sizeof("/v4/") - 1;

    auto tilesetEndIdx = url.find("/", tilesetStartIdx);
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

    auto extensionIdx = url.find(".", basenameIdx);
    if (extensionIdx == std::string::npos || extensionIdx == queryIdx - 1) {
        return url;
    }

    auto tileset = url.substr(tilesetStartIdx, tilesetEndIdx - tilesetStartIdx);
    auto extension = url.substr(extensionIdx + 1, queryIdx - extensionIdx - 1);

#if !defined(__ANDROID__) && !defined(__APPLE__)
    // Replace PNG with WebP.
    if (extension == "png") {
        extension = "webp";
    }
#endif // !defined(__ANDROID__) && !defined(__APPLE__)

    return "mapbox://tiles/" + tileset + "/{z}/{x}/{y}" +
        (type == SourceType::Raster ? "{ratio}" : "") + "." + extension;
}

} // end namespace mapbox
} // end namespace util
} // end namespace mbgl
