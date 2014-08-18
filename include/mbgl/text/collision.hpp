#ifndef MBGL_TEXT_COLLISION
#define MBGL_TEXT_COLLISION

#include <mbgl/text/types.hpp>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wdeprecated-register"
#ifndef __clang__
#pragma GCC diagnostic ignored "-Wunused-local-typedefs"
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#endif
#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point.hpp>
#include <boost/geometry/geometries/box.hpp>
#include <boost/geometry/index/rtree.hpp>
#pragma GCC diagnostic pop

namespace mbgl {

namespace bg = boost::geometry;
namespace bgm = bg::model;
namespace bgi = bg::index;
typedef bgm::point<float, 2, bg::cs::cartesian> Point;
typedef bgm::box<Point> Box;
typedef std::pair<Box, PlacementBox> PlacementValue;
typedef bgi::rtree<PlacementValue, bgi::rstar<16>> Tree;

class Collision {

public:
    Collision(float zoom, float tileExtent, float tileSize, float placementDepth = 1);

    float getPlacementScale(const GlyphBoxes &glyphs, float minPlacementScale, bool avoidEdges);
    PlacementRange getPlacementRange(const GlyphBoxes &glyphs, float placementScale,
                                     bool horizontal);
    void insert(const GlyphBoxes &glyphs, const CollisionAnchor &anchor, float placementScale,
                const PlacementRange &placementRange, bool horizontal);

private:
    Tree hTree;
    Tree cTree;
    PlacementValue leftEdge;
    PlacementValue topEdge;
    PlacementValue rightEdge;
    PlacementValue bottomEdge;

public:
    const float tilePixelRatio;
    const float zoom;
    const float maxPlacementScale;
};
}

#endif
