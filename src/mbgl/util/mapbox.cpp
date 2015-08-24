#include <mbgl/util/mapbox.hpp>

#include <stdexcept>
#include <vector>

namespace mbgl {
namespace util {
namespace mapbox {

const std::string protocol = "mapbox://";
const std::string baseURL = "https://api.mapbox.com/";

bool isMapboxURL(const std::string& url) {
    return url.compare(0, protocol.length(), protocol) == 0;
}

std::vector<std::string> getMapboxURLPathname(const std::string& url) {
    std::vector<std::string> pathname;
    std::size_t startIndex = protocol.length();
    while (startIndex < url.length()) {
        std::size_t endIndex = url.find("/", startIndex);
        if (endIndex == std::string::npos) endIndex = url.length();
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
    std::string user = pathname[1];
    return baseURL + "fonts/v1/" + user + "/{fontstack}/{range}.pbf?access_token=" + accessToken;
}

std::string normalizeTileURL(const std::string& url, const std::string& sourceURL, SourceType sourceType) {
    if (sourceURL.empty() || !isMapboxURL(sourceURL) || sourceType != SourceType::Raster) {
        return url;
    }

    std::string::size_type queryIdx = url.rfind("?");
    // Trim off the right end but never touch anything before the extension dot.
    std::string urlSansParams((queryIdx == std::string::npos) ? url : url.substr(0, queryIdx));

    while (!urlSansParams.empty() && isdigit(urlSansParams.back())) {
        urlSansParams.pop_back();
    }

    std::string::size_type basenameIdx = url.rfind("/", queryIdx);
    std::string::size_type extensionIdx = url.rfind(".", queryIdx);
    if (basenameIdx == std::string::npos || extensionIdx == std::string::npos ||
        basenameIdx > extensionIdx) {
        // No file extension: probably not a file name we can tack a ratio onto.
        return url;
    }

    std::string normalizedURL(url);
    normalizedURL.insert(extensionIdx, "{ratio}");
    return normalizedURL;
}

}
}
}
