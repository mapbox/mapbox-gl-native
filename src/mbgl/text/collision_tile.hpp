#pragma once

#include <mbgl/text/collision_feature.hpp>
#include <mbgl/text/placement_config.hpp>
#include <mbgl/tile/geometry_tile_data.hpp>

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
#ifndef __clang__
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#pragma GCC diagnostic ignored "-Wmisleading-indentation"
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
typedef bgm::point<float, 2, bg::cs::cartesian> CollisionPoint;
typedef bgm::box<CollisionPoint> Box;
typedef std::tuple<Box, CollisionBox, IndexedSubfeature> CollisionTreeBox;
typedef bgi::rtree<CollisionTreeBox, bgi::linear<16, 4>> Tree;

class IndexedSubfeature;

class CollisionTile {
public:
    explicit CollisionTile(PlacementConfig);

    float placeFeature(const CollisionFeature&, bool allowOverlap, bool avoidEdges);
    void insertFeature(CollisionFeature&, float minPlacementScale, bool ignorePlacement);

    std::vector<IndexedSubfeature> queryRenderedSymbols(const GeometryCoordinates&, float scale) const;

    const PlacementConfig config;

    const float minScale = 0.5f;
    const float maxScale = 2.0f;
    float yStretch;

    std::array<float, 4> rotationMatrix;
    std::array<float, 4> reverseRotationMatrix;

private:
    float findPlacementScale(
            const Point<float>& anchor, const CollisionBox& box,
            const Point<float>& blockingAnchor, const CollisionBox& blocking);
    Box getTreeBox(const Point<float>& anchor, const CollisionBox& box, const float scale = 1.0);

    Tree tree;
    Tree ignoredTree;
};

} // namespace mbgl
