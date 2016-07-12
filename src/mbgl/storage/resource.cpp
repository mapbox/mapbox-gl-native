#include <mbgl/storage/resource.hpp>
#include <mbgl/util/string.hpp>
#include <mbgl/util/token.hpp>
#include <mbgl/util/url.hpp>

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

Resource Resource::spriteImage(const std::string& base, float pixelRatio) {
    return Resource {
        Resource::Kind::SpriteImage,
        base + (pixelRatio > 1 ? "@2x" : "") + ".png"
    };
}

Resource Resource::spriteJSON(const std::string& base, float pixelRatio) {
    return Resource {
        Resource::Kind::SpriteJSON,
        base + (pixelRatio > 1 ? "@2x" : "") + ".json"
    };
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
                        Necessity necessity) {
    bool supportsRatio = urlTemplate.find("{ratio}") != std::string::npos;
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
        necessity
    };
}

} // namespace mbgl
