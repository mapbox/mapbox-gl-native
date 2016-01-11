#include <mbgl/util/url.hpp>
#include <mbgl/util/token.hpp>
#include <mbgl/util/string.hpp>
#include <mbgl/map/tile_id.hpp>

#include <cctype>
#include <iomanip>
#include <sstream>
#include <string>
#include <cstdlib>
#include <algorithm>

namespace mbgl {
namespace util {

std::string percentEncode(const std::string& input) {
    std::ostringstream encoded;

    encoded.fill('0');
    encoded << std::hex;

    for (auto c : input) {
        if (std::isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
            encoded << c;
        } else {
            encoded << '%' << std::setw(2) << int(c);
        }
    }

    return encoded.str();
}

std::string percentDecode(const std::string& input) {
    std::string decoded;

    auto it = input.begin();
    const auto end = input.end();
    char hex[3] = "00";

    while (it != end) {
        auto cur = std::find(it, end, '%');
        decoded.append(it, cur);
        it = cur;
        if (cur != end) {
            it += input.copy(hex, 2, cur - input.begin() + 1) + 1;
            decoded += static_cast<char>(std::strtoul(hex, nullptr, 16));
        }
    }

    return decoded;
}

std::string templateTileURL(const std::string& url, const TileID& id, float pixelRatio) {
    return util::replaceTokens(url, [&](const std::string& token) -> std::string {
        if (token == "z") {
            return util::toString(id.sourceZ);
        } else if (token == "x") {
            return util::toString(id.x);
        } else if (token == "y") {
            return util::toString(id.y);
        } else if (token == "prefix") {
            std::string prefix{ 2 };
            prefix[0] = "0123456789abcdef"[id.x % 16];
            prefix[1] = "0123456789abcdef"[id.y % 16];
            return prefix;
        } else if (token == "ratio") {
            return pixelRatio > 1.0 ? "@2x" : "";
        } else {
            return "";
        }
    });
}

} // namespace util
} // namespace mbgl
