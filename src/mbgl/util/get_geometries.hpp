#ifndef MBGL_UTIL_GET_GEOMETRIES
#define MBGL_UTIL_GET_GEOMETRIES

#include <mbgl/map/geometry_tile.hpp>

namespace mbgl {

GeometryCollection getGeometries(const GeometryTileFeature& feature);

} // namespace mbgl

#endif
