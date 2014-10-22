#ifndef MBGL_TEXT_TYPES
#define MBGL_TEXT_TYPES

#include <mbgl/util/vec.hpp>
#include <mbgl/util/rect.hpp>
#include <mbgl/util/optional.hpp>
#include <array>
#include <vector>

namespace mbgl {

typedef vec2<float> CollisionPoint;
typedef vec2<float> CollisionAnchor;

typedef std::array<float, 2> PlacementRange;
typedef float CollisionAngle;
typedef std::vector<CollisionAngle> CollisionAngles;
typedef std::array<CollisionAngle, 2> CollisionRange;
typedef std::vector<CollisionRange> CollisionList;
typedef std::array<CollisionPoint, 4> CollisionCorners;

struct CollisionRect {
    CollisionPoint tl;
    CollisionPoint br;
    inline explicit CollisionRect() {}
    inline explicit CollisionRect(CollisionPoint::Type ax,
                                  CollisionPoint::Type ay,
                                  CollisionPoint::Type bx,
                                  CollisionPoint::Type by)
        : tl(ax, ay), br(bx, by) {}
    inline explicit CollisionRect(const CollisionPoint &tl_,
                                  const CollisionPoint &br_)
        : tl(tl_), br(br_) {}
};

// These are the glyph boxes that we want to have placed.
struct GlyphBox {
    explicit GlyphBox() {}
    explicit GlyphBox(const CollisionRect &box_,
                      const CollisionAnchor &anchor_,
                      float minScale_,
                      float maxScale_,
                      float padding_)
        : box(box_), anchor(anchor_), minScale(minScale_), maxScale(maxScale_), padding(padding_) {}
    explicit GlyphBox(const CollisionRect &box_,
                      float minScale_,
                      float padding_)
        : box(box_), minScale(minScale_), padding(padding_) {}

    CollisionRect box;
    CollisionAnchor anchor;
    float minScale = 0.0f;
    float maxScale = std::numeric_limits<float>::infinity();
    float padding = 0.0f;
    mapbox::util::optional<CollisionRect> hBox;
};

typedef std::vector<GlyphBox> GlyphBoxes;


// TODO: Transform the vec2<float>s to vec2<int16_t> to save bytes
struct PlacedGlyph {
    explicit PlacedGlyph(const vec2<float> &tl_, const vec2<float> &tr_,
                      const vec2<float> &bl_, const vec2<float> &br_,
                      const Rect<uint16_t> &tex_, float angle_, const vec2<float> &anchor_,
                      float minScale_, float maxScale_)
        : tl(tl_),
          tr(tr_),
          bl(bl_),
          br(br_),
          tex(tex_),
          angle(angle_),
          anchor(anchor_),
          minScale(minScale_),
          maxScale(maxScale_) {}

    vec2<float> tl, tr, bl, br;
    Rect<uint16_t> tex;
    float angle;
    vec2<float> anchor;
    float minScale, maxScale;
};

typedef std::vector<PlacedGlyph> PlacedGlyphs;

// These are the placed boxes contained in the rtree.
struct PlacementBox {
    CollisionAnchor anchor;
    CollisionRect box;
    mapbox::util::optional<CollisionRect> hBox;
    PlacementRange placementRange = {{0.0f, 0.0f}};
    float placementScale = 0.0f;
    float maxScale = std::numeric_limits<float>::infinity();
    float padding = 0.0f;
};

struct PlacementProperty {
    explicit PlacementProperty() {}
    explicit PlacementProperty(float zoom_, const PlacementRange &rotationRange_)
        : zoom(zoom_), rotationRange(rotationRange_) {}

    inline operator bool() const {
        return !std::isnan(zoom) && zoom != std::numeric_limits<float>::infinity() &&
               rotationRange[0] != rotationRange[1];
    }

    float zoom = std::numeric_limits<float>::infinity();
    PlacementRange rotationRange = {{0.0f, 0.0f}};
};

}

#endif
