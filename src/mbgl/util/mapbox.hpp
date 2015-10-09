#ifndef MBGL_UTIL_MAPBOX
#define MBGL_UTIL_MAPBOX

#include <string>
#include <mbgl/style/types.hpp>

namespace mbgl {
namespace util {
namespace mapbox {

std::string normalizeSourceURL(const std::string& url, const std::string& accessToken);
std::string normalizeStyleURL(const std::string& url, const std::string& accessToken);
std::string normalizeSpriteURL(const std::string& url, const std::string& accessToken);
std::string normalizeGlyphsURL(const std::string& url, const std::string& accessToken);
std::string normalizeTileURL(const std::string& url, const std::string& sourceURL, SourceType sourceType);

// Canonicalizes Mapbox URLs by removing [a-d] subdomain prefixes, access tokens, and protocol.
// Note that this is close, but not exactly the reverse operation as above, as this retains certain
// information, such as the API version. It is used to cache resources retrieved from the URL, that
// sometimes have multiple valid URLs.
std::string canonicalURL(const std::string &url);

}
}
}

#endif
