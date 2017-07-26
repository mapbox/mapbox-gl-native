#pragma once

#include <mbgl/storage/resource_kind.hpp>
#include <mbgl/util/optional.hpp>
#include <mbgl/util/chrono.hpp>
#include <mbgl/util/font_stack.hpp>
#include <mbgl/util/tileset.hpp>

#include <string>

namespace mbgl {

class Resource {
public:
    struct TileData {
        std::string urlTemplate;
        uint8_t pixelRatio;
        int32_t x;
        int32_t y;
        int8_t z;
    };

    enum Necessity : bool {
        Optional = false,
        Required = true,
    };

    Resource(ResourceKind kind_, std::string url_, optional<TileData> tileData_ = {}, Necessity necessity_ = Required)
        : kind(kind_),
          necessity(necessity_),
          url(std::move(url_)),
          tileData(std::move(tileData_)) {
    }

    static Resource style(const std::string& url);
    static Resource source(const std::string& url);
    static Resource tile(const std::string& urlTemplate,
                         float pixelRatio,
                         int32_t x,
                         int32_t y,
                         int8_t z,
                         Tileset::Scheme scheme,
                         Necessity = Required);
    static Resource glyphs(const std::string& urlTemplate,
                           const FontStack& fontStack,
                           const std::pair<uint16_t, uint16_t>& glyphRange);
    static Resource spriteImage(const std::string& base, float pixelRatio);
    static Resource spriteJSON(const std::string& base, float pixelRatio);
    static Resource image(const std::string& url);

    ResourceKind kind;
    Necessity necessity;
    std::string url;

    // Includes auxiliary data if this is a tile request.
    optional<TileData> tileData;

    optional<Timestamp> priorModified = {};
    optional<Timestamp> priorExpires = {};
    optional<std::string> priorEtag = {};
};

} // namespace mbgl
