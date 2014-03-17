#ifndef LLMR_TEXT_COLLISION
#define LLMR_TEXT_COLLISION

#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point.hpp>
#include <boost/geometry/geometries/box.hpp>
#include <boost/geometry/index/rtree.hpp>

#include <llmr/text/rotation_range.hpp>

namespace llmr {

// These are the glyph boxes that we want to have placed.
struct GlyphBox {
    GlyphBox() {}
    GlyphBox(const CollisionRect &bbox, const CollisionRect &box,
             float minScale)
        : bbox(bbox), box(box), minScale(minScale) {}

    CollisionAnchor anchor;
    CollisionRect bbox;
    CollisionRect box;
    bool rotate = false;
    float minScale = 0.0f;
    float maxScale = std::numeric_limits<float>::infinity();
};

typedef std::vector<GlyphBox> GlyphBoxes;

// These are the placed boxes contained in the rtree.
struct PlacementBox {
    CollisionAnchor anchor;
    CollisionRect bbox;
    CollisionRect box;
    bool rotate = false;
    PlacementRange placementRange = {{0.0f, 0.0f}};
    float placementScale = 0.0f;
    float maxScale = std::numeric_limits<float>::infinity();
    float padding = 0.0f;
};

struct Placement {
    Placement() {}
    Placement(float zoom, const PlacementRange &rotationRange)
        : zoom(zoom), rotationRange(rotationRange) {}

    float zoom = -1.0f;
    PlacementRange rotationRange = {{0.0f, 0.0f}};
};

class Collision {
    typedef boost::geometry::model::point<float, 2,
                                          boost::geometry::cs::cartesian> Point;
    typedef boost::geometry::model::box<Point> Box;
    typedef std::pair<Box, PlacementBox> PlacementValue;
    typedef boost::geometry::index::rtree<
        PlacementValue, boost::geometry::index::quadratic<16>> Tree;

  public:
    Collision();

    Placement place(const GlyphBoxes &boxes, const CollisionAnchor &anchor,
               float minPlacementScale, float maxPlacementScale, float padding,
               bool horizontal);
    float getPlacementScale(const GlyphBoxes &glyphs, float minPlacementScale,
                            float maxPlacementScale, float pad);
    PlacementRange getPlacementRange(const GlyphBoxes &glyphs,
                                     float placementScale);
    void insert(const GlyphBoxes &glyphs, const CollisionAnchor &anchor,
                float placementScale, const PlacementRange &placementRange,
                bool horizontal, float padding);

  private:
    Tree tree;
};
}

#endif