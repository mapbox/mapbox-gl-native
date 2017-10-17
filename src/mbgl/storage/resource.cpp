#include <mapbox/geometry/box.hpp>
#include <mbgl/storage/resource.hpp>
#include <mbgl/util/constants.hpp>
#include <mbgl/util/string.hpp>
#include <mbgl/util/token.hpp>
#include <mbgl/util/url.hpp>

#include <cmath>

namespace mbgl {

static std::string getQuadKey(int32_t x, int32_t y, int8_t z) {
    std::string quadKey;
    quadKey.reserve(z);
    int32_t mask;
    for (int8_t i = z; i > 0; i--) {
        mask = 1 << (i - 1);
        quadKey += '0' + ((x & mask ? 1 : 0) + (y & mask ? 2 : 0));
    }
    return quadKey;
}

static mapbox::geometry::point<double> getMercCoord(int32_t x, int32_t y, int8_t z) {
    double resolution = (util::M2PI * util::EARTH_RADIUS_M / 256) / std::pow(2.0f, z);
    return {
        x * resolution - util::M2PI * util::EARTH_RADIUS_M / 2,
        y * resolution - util::M2PI * util::EARTH_RADIUS_M / 2,
    };
}

static std::string getTileBBox(int32_t x, int32_t y, int8_t z) {
    // Alter the y for the Google/OSM tile scheme.
    y = std::pow(2.0f, z) - y - 1;

    auto min = getMercCoord(x * 256, y * 256, z);
    auto max = getMercCoord((x + 1) * 256, (y + 1) * 256, z);

    return (util::toString(min.x) + "," + util::toString(min.y) + "," +
            util::toString(max.x) + "," + util::toString(max.y));
}

Resource Resource::style(const std::string& url) {
    return Resource {
        Resource::Kind::Style,
        url
    };
}

Resource Resource::source(const std::string& url) {
    return Resource {
        Resource::Kind::Source,
        url
    };
}

Resource Resource::image(const std::string& url) {
    return Resource {
        Resource::Kind::Image,
        url
    };
}

Resource Resource::spriteImage(const std::string& base, float pixelRatio) {
    util::URL url(base);
    return Resource{ Resource::Kind::SpriteImage,
                     base.substr(0, url.path.first + url.path.second) +
                         (pixelRatio > 1 ? "@2x" : "") + ".png" +
                         base.substr(url.query.first, url.query.second) };
}

Resource Resource::spriteJSON(const std::string& base, float pixelRatio) {
    util::URL url(base);
    return Resource{ Resource::Kind::SpriteJSON,
                     base.substr(0, url.path.first + url.path.second) +
                         (pixelRatio > 1 ? "@2x" : "") + ".json" +
                         base.substr(url.query.first, url.query.second) };
}

Resource Resource::glyphs(const std::string& urlTemplate, const FontStack& fontStack, const std::pair<uint16_t, uint16_t>& glyphRange) {
    return Resource {
        Resource::Kind::Glyphs,
        util::replaceTokens(urlTemplate, [&](const std::string& token) {
            if (token == "fontstack") {
                return util::percentEncode(fontStackToString(fontStack));
            } else if (token == "range") {
                return util::toString(glyphRange.first) + "-" + util::toString(glyphRange.second);
            } else {
                return std::string();
            }
        })
    };
}

Resource Resource::tile(const std::string& urlTemplate,
                        float pixelRatio,
                        int32_t x,
                        int32_t y,
                        int8_t z,
                        Tileset::Scheme scheme,
                        LoadingMethod loadingMethod) {
    bool supportsRatio = urlTemplate.find("{ratio}") != std::string::npos;
    if (scheme == Tileset::Scheme::TMS) {
        y = (1 << z) - y - 1;
    }
    return Resource {
        Resource::Kind::Tile,
        util::replaceTokens(urlTemplate, [&](const std::string& token) {
            if (token == "z") {
                return util::toString(z);
            } else if (token == "x") {
                return util::toString(x);
            } else if (token == "y") {
                return util::toString(y);
            } else if (token == "quadkey") {
                return getQuadKey(x, y, z);
            } else if (token == "bbox-epsg-3857") {
                return getTileBBox(x, y, z);
            } else if (token == "prefix") {
                std::string prefix{ 2 };
                prefix[0] = "0123456789abcdef"[x % 16];
                prefix[1] = "0123456789abcdef"[y % 16];
                return prefix;
            } else if (token == "ratio") {
                return std::string(pixelRatio > 1.0 ? "@2x" : "");
            } else {
                return std::string();
            }
        }),
        Resource::TileData {
            urlTemplate,
            uint8_t(supportsRatio && pixelRatio > 1.0 ? 2 : 1),
            x,
            y,
            z
        },
        loadingMethod
    };
}

} // namespace mbgl
