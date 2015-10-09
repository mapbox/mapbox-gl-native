#include <mbgl/util/mapbox.hpp>

#include <stdexcept>
#include <vector>
#include <iostream>

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
    std::string fontstack = pathname[2];
    std::string range = pathname[3];

    return baseURL + "fonts/v1/" + user + "/" + fontstack + "/" + range + "?access_token=" + accessToken;
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


std::string removeAccessTokenFromURL(const std::string &url) {
    const size_t token_start = url.find("access_token=");
    // Ensure that token exists, isn't at the front and is preceded by either & or ?.
    if (token_start == std::string::npos || token_start == 0 || !(url[token_start - 1] == '&' || url[token_start - 1] == '?')) {
        return url;
    }

    const size_t token_end = url.find_first_of('&', token_start);
    if (token_end == std::string::npos) {
        // The token is the last query argument. We slice away the "&access_token=..." part
        return url.substr(0, token_start - 1);
    } else {
        // We slice away the "access_token=...&" part.
        return url.substr(0, token_start) + url.substr(token_end + 1);
    }
}

namespace {

std::string convertMapboxDomainsToProtocol(const std::string &url) {
    const size_t protocol_separator = url.find("://");
    if (protocol_separator == std::string::npos) {
        return url;
    }

    const std::string protocol = url.substr(0, protocol_separator);
    if (!(protocol == "http" || protocol == "https")) {
        return url;
    }

    const size_t domain_begin = protocol_separator + 3;
    const size_t path_separator = url.find("/", domain_begin);
    if (path_separator == std::string::npos) {
        return url;
    }

    const std::string domain = url.substr(domain_begin, path_separator - domain_begin);
    if (domain == "api.mapbox.com" || domain.find(".tiles.mapbox.com") != std::string::npos) {
        return std::string{ "mapbox://" } + url.substr(path_separator + 1);
    } else {
        return url;
    }
}

} // end namespace

std::string canonicalURL(const std::string &url) {
    return removeAccessTokenFromURL(convertMapboxDomainsToProtocol(url));
}

} // end namespace mapbox
} // end namespace util
} // end namespace mbgl
