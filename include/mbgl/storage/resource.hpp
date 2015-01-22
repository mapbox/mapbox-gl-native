#ifndef MBGL_STORAGE_RESOURCE
#define MBGL_STORAGE_RESOURCE

#include <string>
#include <functional>

namespace mbgl {

struct Resource {
    enum Kind : uint8_t {
        Unknown = 0,
        Tile = 1,
        Glyphs = 2,
        Image = 3,
        JSON = 4,
    };

    const Kind kind;
    const std::string url;

    inline bool operator==(const Resource &res) const {
        return kind == res.kind && url == res.url;
    }

    struct Hash {
        std::size_t operator()(Resource const& r) const {
            return std::hash<std::string>()(r.url) ^ (std::hash<uint8_t>()(r.kind) << 1);
        }
    };

};

}

#endif
