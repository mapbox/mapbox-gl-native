#ifndef MBGL_STORAGE_RESOURCE_TYPE
#define MBGL_STORAGE_RESOURCE_TYPE

#include <cstdint>

namespace mbgl {

enum class ResourceType : uint8_t {
    Unknown = 0,
    Tile = 1,
    Glyphs = 2,
    Image = 3,
    JSON = 4
};

}

#endif
