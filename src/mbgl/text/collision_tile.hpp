#ifndef MBGL_TEXT_COLLISION_TILE
#define MBGL_TEXT_COLLISION_TILE

#include <mbgl/text/collision_feature.hpp>

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
    typedef bgi::rtree<CollisionTreeBox, bgi::linear<16,4>> Tree;

class CollisionTile {

    public:
    inline explicit CollisionTile(float _zoom, float tileExtent, float tileSize, float angle_, float pitch_, bool debug_) :
        zoom(_zoom), tilePixelRatio(tileExtent / tileSize), debug(debug_) { reset(angle_, pitch_); }

    void reset(const float angle, const float pitch);
    float placeFeature(const CollisionFeature &feature);
    void insertFeature(CollisionFeature &feature, const float minPlacementScale);

    void setDebug(bool debug_) { debug = debug_; }
    bool getDebug() { return debug; }

    const float zoom;
    const float tilePixelRatio;
    float angle = 0;

    const float minScale = 0.5f;
    const float maxScale = 2.0f;
    float yStretch;

    private:

    Box getTreeBox(const vec2<float> &anchor, const CollisionBox &box);

    Tree tree;
    std::array<float, 4> rotationMatrix;
    bool debug;

};
}

#endif
