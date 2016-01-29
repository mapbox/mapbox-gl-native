#include <mbgl/storage/resource.hpp>
#include <mbgl/util/string.hpp>
#include <mbgl/util/token.hpp>
#include <mbgl/util/url.hpp>

namespace mbgl {

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

Resource Resource::glyphs(const std::string& urlTemplate, const std::string& fontStack, const std::pair<uint16_t, uint16_t>& glyphRange) {
    return Resource {
        Resource::Kind::Glyphs,
        util::replaceTokens(urlTemplate, [&](const std::string& token) {
            if (token == "fontstack") {
                return util::percentEncode(fontStack);
            } else if (token == "range") {
                return util::toString(glyphRange.first) + "-" + util::toString(glyphRange.second);
            } else {
                return std::string();
            }
        })
    };
}

Resource Resource::tile(const std::string& urlTemplate, float pixelRatio, int32_t x, int32_t y, int8_t z) {
    return Resource {
        Resource::Kind::Tile,
        util::replaceTokens(urlTemplate, [&](const std::string& token) {
            if (token == "z") {
                return util::toString(z);
            } else if (token == "x") {
                return util::toString(x);
            } else if (token == "y") {
                return util::toString(y);
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
            pixelRatio,
            x,
            y,
            z
        }
    };
}

}
