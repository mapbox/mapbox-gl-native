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

#include <mbgl/map/transform_state.hpp>

namespace mbgl {

namespace bg = boost::geometry;
namespace bgm = bg::model;
namespace bgi = bg::index;
using CollisionPoint = bgm::point<float, 2, bg::cs::cartesian>;
using Box = bgm::box<CollisionPoint>;
using CollisionTreeBox = std::tuple<Box, CollisionBox, IndexedSubfeature>;
using Tree = bgi::rtree<CollisionTreeBox, bgi::linear<16, 4>>;

class IndexedSubfeature;
class PlacedSymbol;

struct TileDistance;

class CollisionIndex {
public:
    explicit CollisionIndex(const TransformState&);

    bool placeFeature(CollisionFeature& feature,
                                      const mat4& posMatrix,
                                      const mat4& labelPlaneMatrix,
                                      const float textPixelRatio,
                                      PlacedSymbol& symbol,
                                      const float scale,
                                      const float fontSize,
                                      const bool allowOverlap,
                                      const bool pitchWithMap,
                                      const bool collisionDebug);

    void insertFeature(CollisionFeature& feature, bool ignorePlacement);

    std::vector<IndexedSubfeature> queryRenderedSymbols(const GeometryCoordinates&, const UnwrappedTileID& tileID, const float textPixelRatio) const;

    
private:
    bool placeLineFeature(CollisionFeature& feature,
                                  const mat4& posMatrix,
                                  const mat4& labelPlaneMatrix,
                                  const float textPixelRatio,
                                  PlacedSymbol& symbol,
                                  const float scale,
                                  const float fontSize,
                                  const bool allowOverlap,
                                  const bool pitchWithMap,
                                  const bool collisionDebug);

    Box getTreeBox(const CollisionBox& box) const;
    
    float approximateTileDistance(const TileDistance& tileDistance, const float lastSegmentAngle, const float pixelsToTileUnits, const float cameraToAnchorDistance, const bool pitchWithMap);
    
    std::pair<float,float> projectAnchor(const mat4& posMatrix, const Point<float>& point) const;
    std::pair<Point<float>,float> projectAndGetPerspectiveRatio(const mat4& posMatrix, const Point<float>& point) const;
    Point<float> projectPoint(const mat4& posMatrix, const Point<float>& point) const;

    TransformState transformState;
    float pitchFactor;

    Tree tree;
    Tree ignoredTree;
};

} // namespace mbgl
