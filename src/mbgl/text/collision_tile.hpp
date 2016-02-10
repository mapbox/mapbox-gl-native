#ifndef MBGL_TEXT_COLLISION_TILE
#define MBGL_TEXT_COLLISION_TILE

#include <mbgl/text/collision_feature.hpp>
#include <mbgl/text/placement_config.hpp>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-function"
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wunused-variable"
#pragma GCC diagnostic ignored "-Wshadow"
#ifdef __clang__
#pragma GCC diagnostic ignored "-Wunknown-pragmas"
#endif
#pragma GCC diagnostic ignored "-Wpragmas"
#pragma GCC diagnostic ignored "-Wdeprecated-register"
#pragma GCC diagnostic ignored "-Wshorten-64-to-32"
#pragma GCC diagnostic ignored "-Wunused-local-typedefs"
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point.hpp>
#include <boost/geometry/geometries/box.hpp>
#include <boost/geometry/index/rtree.hpp>
#pragma GCC diagnostic pop

namespace mbgl {

namespace bg = boost::geometry;
namespace bgm = bg::model;
namespace bgi = bg::index;
typedef bgm::point<float, 2, bg::cs::cartesian> CollisionPoint;
typedef bgm::box<CollisionPoint> Box;
typedef std::pair<Box, CollisionBox> CollisionTreeBox;
typedef bgi::rtree<CollisionTreeBox, bgi::linear<16, 4>> Tree;

class CollisionTile {
public:
    explicit CollisionTile(PlacementConfig);

    float placeFeature(const CollisionFeature& feature, const bool allowOverlap, const bool avoidEdges);
    void insertFeature(CollisionFeature& feature, const float minPlacementScale);

    const PlacementConfig config;

    const float minScale = 0.5f;
    const float maxScale = 2.0f;
    float yStretch;

private:
    float findPlacementScale(float minPlacementScale,
            const vec2<float>& anchor, const CollisionBox& box,
            const vec2<float>& blockingAnchor, const CollisionBox& blocking);
    Box getTreeBox(const vec2<float>& anchor, const CollisionBox& box);

    Tree tree;
    std::array<float, 4> rotationMatrix;
    std::array<float, 4> reverseRotationMatrix;
    std::array<CollisionBox, 4> edges;
};

} // namespace mbgl

#endif
