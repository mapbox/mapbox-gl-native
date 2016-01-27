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

    Resource(Kind kind_, const std::string& url_)
        : kind(kind_),
          url(url_) {
    }

    Kind kind;
    std::string url;

    optional<SystemTimePoint> priorModified;
    optional<SystemTimePoint> priorExpires;
    optional<std::string> priorEtag;

    // Includes auxiliary data if this is a tile request.

    struct TileData {
        int32_t x;
        int32_t y;
        int8_t z;
        float pixelRatio;
    };

    optional<TileData> tileData;
};

} // namespace mbgl

#endif
