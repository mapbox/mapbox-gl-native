#ifndef MBGL_STORAGE_RESOURCE
#define MBGL_STORAGE_RESOURCE

#include <mbgl/storage/response.hpp>
#include <mbgl/util/optional.hpp>

#include <string>

namespace mbgl {

class Resource {
public:
    enum Kind : uint8_t {
        Unknown = 0,
        Style,
        Source,
        Tile,
        Glyphs,
        SpriteImage,
        SpriteJSON
    };

    struct TileData {
        std::string urlTemplate;
        uint8_t pixelRatio;
        int32_t x;
        int32_t y;
        int8_t z;
    };

    Resource(Kind kind_, const std::string& url_, optional<TileData> tileData_ = {})
        : kind(kind_),
          url(url_),
          tileData(tileData_) {
    }

    static Resource style(const std::string& url);
    static Resource source(const std::string& url);
    static Resource tile(const std::string& urlTemplate,
                         float pixelRatio,
                         int32_t x,
                         int32_t y,
                         int8_t z);
    static Resource glyphs(const std::string& urlTemplate,
                           const std::string& fontStack,
                           const std::pair<uint16_t, uint16_t>& glyphRange);
    static Resource spriteImage(const std::string& base, float pixelRatio);
    static Resource spriteJSON(const std::string& base, float pixelRatio);

    Kind kind;
    std::string url;

    // Includes auxiliary data if this is a tile request.
    optional<TileData> tileData;

    optional<SystemTimePoint> priorModified = {};
    optional<SystemTimePoint> priorExpires = {};
    optional<std::string> priorEtag = {};
};

} // namespace mbgl

#endif
