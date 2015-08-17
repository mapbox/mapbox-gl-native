#ifndef MBGL_UTIL_TILE_COORDINATE
#define MBGL_UTIL_TILE_COORDINATE

#include <mbgl/util/vec.hpp>

namespace mbgl {

struct TileCoordinate {
    float column;
    float row;
    float zoom;

    TileCoordinate(float column_, float row_, float zoom_) :
        column(column_), row(row_), zoom(zoom_) {}

    TileCoordinate zoomTo(float targetZoom) {
        float scale = std::pow(2, targetZoom - zoom);
        return { column * scale, row * scale, targetZoom };
    }

    TileCoordinate operator-(TileCoordinate c) {
        c = c.zoomTo(zoom);
        return { column - c.column, row - c.row, zoom };
    };
};

}

#endif
