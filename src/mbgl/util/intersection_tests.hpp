#pragma once

#include <mbgl/tile/geometry_tile.hpp>

namespace mbgl {
namespace util {

bool multiPolygonIntersectsBufferedMultiPoint(const GeometryCollection&, const GeometryCollection&, float radius);
bool multiPolygonIntersectsBufferedMultiLine(const GeometryCollection&, const GeometryCollection&, float radius);
bool multiPolygonIntersectsMultiPolygon(const GeometryCollection&, const GeometryCollection&);

}
} // namespace mbgl
