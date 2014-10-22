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
    inline explicit CollisionRect(const CollisionPoint &tl,
                                  const CollisionPoint &br)
        : tl(tl), br(br) {}
};

// These are the glyph boxes that we want to have placed.
struct GlyphBox {
    explicit GlyphBox() {}
    explicit GlyphBox(const CollisionRect &box,
                      const CollisionAnchor &anchor,
                      float minScale,
                      float maxScale,
                      float padding)
        : box(box), anchor(anchor), minScale(minScale), maxScale(maxScale), padding(padding) {}
    explicit GlyphBox(const CollisionRect &box,
                      float minScale,
                      float padding)
        : box(box), minScale(minScale), padding(padding) {}

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
    explicit PlacedGlyph(const vec2<float> &tl, const vec2<float> &tr,
                      const vec2<float> &bl, const vec2<float> &br,
                      const Rect<uint16_t> &tex, float angle, const vec2<float> &anchor,
                      float minScale, float maxScale)
        : tl(tl),
          tr(tr),
          bl(bl),
          br(br),
          tex(tex),
          angle(angle),
          anchor(anchor),
          minScale(minScale),
          maxScale(maxScale) {}

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
    explicit PlacementProperty(float zoom, const PlacementRange &rotationRange)
        : zoom(zoom), rotationRange(rotationRange) {}

    inline operator bool() const {
        return !std::isnan(zoom) && zoom != std::numeric_limits<float>::infinity() &&
               rotationRange[0] != rotationRange[1];
    }

    float zoom = std::numeric_limits<float>::infinity();
    PlacementRange rotationRange = {{0.0f, 0.0f}};
};

}

#endif
