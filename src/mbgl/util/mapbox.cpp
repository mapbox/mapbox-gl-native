#include <mbgl/util/mapbox.hpp>

#include <stdexcept>

namespace mbgl {
namespace util {
namespace mapbox {

const std::string mapbox = "mapbox://";

std::string normalizeURL(const std::string& url, const std::string& accessToken) {
    if (accessToken.empty())
        throw std::runtime_error("You must provide a Mapbox API access token for Mapbox tile sources");

    return std::string("https://api.tiles.mapbox.com/v4/")
        + url.substr(mapbox.length())
        + "?access_token="
        + accessToken;
}

std::string normalizeSourceURL(const std::string& url, const std::string& accessToken) {
    if (url.compare(0, mapbox.length(), mapbox) != 0)
        return url;

    std::string result = normalizeURL(url + ".json", accessToken);

    // TileJSON requests need a secure flag appended to their URLs so
    // that the server knows to send SSL-ified resource references.
    if (url.compare(0, 5, "https") == 0)
        result += "&secure";

    return result;
}

std::string normalizeGlyphsURL(const std::string& url, const std::string& accessToken) {
    if (url.compare(0, mapbox.length(), mapbox) != 0)
        return url;

    return normalizeURL(url, accessToken);
}

std::string normalizeTileURL(const std::string& url, const std::string& sourceURL) {
    if (sourceURL.empty() || sourceURL.compare(0, mapbox.length(), mapbox) != 0)
        return url;
    
    std::string::size_type queryIdx = url.rfind("?");
    // Trim off the right end but never touch anything before the extension dot.
    std::string urlSansParams((queryIdx == std::string::npos) ? url : url.substr(0, queryIdx));
    
    while (!urlSansParams.empty() && isdigit(urlSansParams.back()))
        urlSansParams.pop_back();
    
    std::string::size_type extensionIdx = urlSansParams.length() - 4;
    if (extensionIdx <= 0)
        return url;
    std::string extension = urlSansParams.substr(extensionIdx);
    if (extension.compare(".png") != 0 && extension.compare(".jpg") != 0)
        return url;
    
    std::string normalizedURL(url);
    normalizedURL.insert(extensionIdx, "{ratio}");
    return normalizedURL;
}

}
}
}
