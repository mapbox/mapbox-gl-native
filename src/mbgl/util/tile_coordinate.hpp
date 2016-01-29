#ifndef MBGL_UTIL_TILE_COORDINATE
#define MBGL_UTIL_TILE_COORDINATE

#include <mbgl/util/vec.hpp>

namespace mbgl {

class TileCoordinate {
public:
    double column;
    double row;
    double zoom;
};

} // namespace mbgl

#endif
