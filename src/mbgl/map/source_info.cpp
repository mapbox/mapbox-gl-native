#include <mbgl/map/source_info.hpp>
#include <mbgl/map/tile_id.hpp>
#include <mbgl/util/mapbox.hpp>
#include <mbgl/util/string.hpp>
#include <mbgl/util/token.hpp>

namespace mbgl {

std::string SourceInfo::tileURL(const TileID& id, float pixelRatio) const {
    std::string result = tiles.at(0);
    result = util::mapbox::normalizeTileURL(result, url, type);
    result = util::replaceTokens(result, [&](const std::string& token) -> std::string {
        if (token == "z") {
            return util::toString(std::min(id.z, static_cast<int8_t>(max_zoom)));
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
    return result;
}

} // namespace mbgl
