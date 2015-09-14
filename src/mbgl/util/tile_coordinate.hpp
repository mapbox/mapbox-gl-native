#ifndef MBGL_UTIL_TILE_COORDINATE
#define MBGL_UTIL_TILE_COORDINATE

#include <mbgl/util/vec.hpp>

namespace mbgl {

struct TileCoordinate {
    double column;
    double row;
    double zoom;

    TileCoordinate(double column_, double row_, double zoom_) :
        column(column_), row(row_), zoom(zoom_) {}

    TileCoordinate zoomTo(double targetZoom) {
        double scale = std::pow(2, targetZoom - zoom);
        return { column * scale, row * scale, targetZoom };
    }

    TileCoordinate operator-(TileCoordinate c) {
        c = c.zoomTo(zoom);
        return { column - c.column, row - c.row, zoom };
    };
};

}

#endif
