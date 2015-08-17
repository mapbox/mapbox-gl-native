#ifndef MBGL_UTIL_BOX
#define MBGL_UTIL_BOX

#include <mbgl/util/tile_coordinate.hpp>

namespace mbgl {

struct box {
    box(TileCoordinate tl_, TileCoordinate tr_, TileCoordinate br_, TileCoordinate bl_) :
        tl(tl_), tr(tr_), br(br_), bl(bl_) {}
    TileCoordinate tl, tr, br, bl;
};

}

#endif
