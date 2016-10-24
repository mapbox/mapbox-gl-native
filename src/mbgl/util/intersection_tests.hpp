#pragma once

#include <mbgl/tile/geometry_tile_data.hpp>

namespace mbgl {
namespace util {

bool polygonIntersectsBufferedMultiPoint(const GeometryCoordinates&, const GeometryCollection&, float radius);
bool polygonIntersectsBufferedMultiLine(const GeometryCoordinates&, const GeometryCollection&, float radius);
bool polygonIntersectsPolygon(const GeometryCoordinates&, const GeometryCoordinates&);
bool polygonIntersectsMultiPolygon(const GeometryCoordinates&, const GeometryCollection&);

} // namespace util
} // namespace mbgl
