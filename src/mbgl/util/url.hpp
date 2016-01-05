#ifndef MBGL_UTIL_URL
#define MBGL_UTIL_URL

#include <string>

namespace mbgl {

class TileID;

namespace util {

std::string percentEncode(const std::string&);
std::string percentDecode(const std::string&);

std::string templateTileURL(const std::string& url, const TileID& id, float pixelRatio = 1);

bool isAssetURL(const std::string&);

} // namespace util
} // namespace mbgl

#endif
