#ifndef MBGL_UTIL_INTERSECTION_TESTS
#define MBGL_UTIL_INTERSECTION_TESTS

#include <mbgl/tile/geometry_tile.hpp>

namespace mbgl {
namespace util {

bool multiPolygonIntersectsBufferedMultiPoint(const GeometryCollection&, const GeometryCollection&, float radius);
bool multiPolygonIntersectsBufferedMultiLine(const GeometryCollection&, const GeometryCollection&, float radius);
bool multiPolygonIntersectsMultiPolygon(const GeometryCollection&, const GeometryCollection&);

}
} // namespace mbgl

#endif
