#ifndef MBGL_TEXT_COLLISION
#define MBGL_TEXT_COLLISION

#include <mbgl/text/types.hpp>

namespace mbgl {

class Collision {

public:
    Collision(float zoom, float tileExtent, float tileSize, float placementDepth = 1);
    ~Collision();

    PlacementProperty place(const GlyphBoxes &boxes, const CollisionAnchor &anchor,
                            float minPlacementScale, float maxPlacementScale, float padding,
                            bool horizontal, bool allowOverlap, bool ignorePlacement);
    float getPlacementScale(const GlyphBoxes &glyphs, float minPlacementScale,
                            float maxPlacementScale, float pad);
    PlacementRange getPlacementRange(const GlyphBoxes &glyphs,
                                     float placementScale, bool horizontal);
    void insert(const GlyphBoxes &glyphs, const CollisionAnchor &anchor,
                float placementScale, const PlacementRange &placementRange,
                bool horizontal, float padding);

private:
    void *hTree;
    void *cTree;
    const float tilePixelRatio;
    const float zoom;
    const float maxPlacementScale;
};
}

#endif
