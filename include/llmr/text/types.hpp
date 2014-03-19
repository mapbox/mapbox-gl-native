#ifndef LLMR_TEXT_TYPES
#define LLMR_TEXT_TYPES

#include <llmr/util/vec.hpp>
#include <llmr/util/rect.hpp>
#include <array>
#include <vector>

namespace llmr {

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
    explicit GlyphBox(const CollisionRect &bbox, const CollisionRect &box,
                      float minScale)
        : bbox(bbox), box(box), minScale(minScale) {}
    explicit GlyphBox(const CollisionRect &box, float minScale, float maxScale,
                      const CollisionAnchor &anchor, bool rotate)
        : anchor(anchor),
          box(box),
          rotate(rotate),
          minScale(minScale),
          maxScale(maxScale) {}

    CollisionAnchor anchor;
    CollisionRect bbox;
    CollisionRect box;
    bool rotate = false;
    float minScale = 0.0f;
    float maxScale = std::numeric_limits<float>::infinity();
};

typedef std::vector<GlyphBox> GlyphBoxes;


// TODO: Transform the vec2<float>s to vec2<int16_t> to save bytes
struct PlacedGlyph {
    explicit PlacedGlyph(const vec2<float> &tl, const vec2<float> &tr,
                      const vec2<float> &bl, const vec2<float> &br,
                      const Rect<uint16_t> &tex, uint32_t width,
                      uint32_t height, float angle, const GlyphBox &glyphBox)
        : tl(tl),
          tr(tr),
          bl(bl),
          br(br),
          tex(tex),
          width(width),
          height(height),
          angle(angle),
          glyphBox(glyphBox) {}

    vec2<float> tl, tr, bl, br;
    Rect<uint16_t> tex;
    uint32_t width, height;
    float angle;
    GlyphBox glyphBox;
};

typedef std::vector<PlacedGlyph> PlacedGlyphs;

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

struct PlacementProperty {
    explicit PlacementProperty() {}
    explicit PlacementProperty(float zoom, const PlacementRange &rotationRange)
        : zoom(zoom), rotationRange(rotationRange) {}

    float zoom = -1.0f;
    PlacementRange rotationRange = {{0.0f, 0.0f}};
};

}

#endif
