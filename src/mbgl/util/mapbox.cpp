#include <mbgl/util/mapbox.hpp>

#include <stdexcept>

namespace mbgl {
namespace util {
namespace mapbox {

const std::string protocol = "mapbox://";
const std::string baseURL = "https://api.mapbox.com/";
const std::string draft = "draft";

struct MapboxURLParts {
    std::string endpoint;
    std::string user;
    std::string id;
    bool draft;
};

bool isMapboxURL(const std::string& url) {
    return url.compare(0, protocol.length(), protocol) == 0;
}

void parseMapboxURL(const std::string& url, MapboxURLParts &parts) {
    std::size_t endpointEndIndex = url.find('/', protocol.length());
    std::size_t userStartIndex = endpointEndIndex + 1;
    std::size_t userEndIndex = url.find('/', userStartIndex);
    std::size_t idStartIndex = userEndIndex + 1;
    std::size_t idEndIndex = url.find('/', idStartIndex);

    parts.endpoint = url.substr(protocol.length(), endpointEndIndex - protocol.length());
    parts.user = url.substr(userStartIndex, userEndIndex - userStartIndex);
    parts.id = url.substr(idStartIndex, idEndIndex - idStartIndex);

    parts.draft = url.compare(idEndIndex + 1, draft.length(), draft) == 0;
}


std::string normalizeSourceURL(const std::string& url, const std::string& accessToken) {
    if (!isMapboxURL(url)) {
        return url;
    } else {
        if (accessToken.empty()) {
            throw std::runtime_error("You must provide a Mapbox API access token for Mapbox tile sources");
        } else {
            return baseURL + "v4/" + url.substr(protocol.length()) + ".json?access_token=" + accessToken + "&secure";
        }
    }
}

std::string normalizeStyleURL(const std::string& url, const std::string& accessToken) {
    if (!isMapboxURL(url)) {
        return url;
    } else {
        static MapboxURLParts parts;
        parseMapboxURL(url, parts);
        return baseURL + "styles/v1/" + parts.user + "/" + parts.id + (parts.draft ? "/draft" : "") + "?access_token=" + accessToken;
    }
}

std::string normalizeSpriteURL(const std::string& url, const std::string& format, const std::string& extension, const std::string& accessToken) {
    if (!isMapboxURL(url)) {
        return url + format + extension;
    } else {
        static MapboxURLParts parts;
        parseMapboxURL(url, parts);
        return baseURL + "styles/v1/" + parts.user + "/" + parts.id + (parts.draft ? "/draft" : "") + "/sprite" + format + extension + "?access_token=" + accessToken;
    }
}

std::string normalizeGlyphsURL(const std::string& url, const std::string& accessToken) {
    if (!isMapboxURL(url)) {
        return url;
    } else {
        static MapboxURLParts parts;
        parseMapboxURL(url, parts);
        return baseURL + "fonts/v1/" + parts.user + "/{fontstack}/{range}.pbf?access_token=" + accessToken;
    }
}

std::string normalizeTileURL(const std::string& url, const std::string& sourceURL, SourceType sourceType) {
    if (sourceURL.empty() || sourceURL.compare(0, protocol.length(), protocol) != 0 ||
        sourceType != SourceType::Raster) {
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
