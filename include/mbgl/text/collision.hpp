#ifndef MBGL_TEXT_COLLISION
#define MBGL_TEXT_COLLISION

#include <mbgl/text/types.hpp>

namespace mbgl {

class Collision {

public:
    Collision();
    ~Collision();

    PlacementProperty place(const GlyphBoxes &boxes,
                            const CollisionAnchor &anchor,
                            float minPlacementScale, float maxPlacementScale,
                            float padding, bool horizontal, bool alwaysVisible);
    float getPlacementScale(const GlyphBoxes &glyphs, float minPlacementScale,
                            float maxPlacementScale, float pad);
    PlacementRange getPlacementRange(const GlyphBoxes &glyphs,
                                     float placementScale, bool horizontal);
    void insert(const GlyphBoxes &glyphs, const CollisionAnchor &anchor,
                float placementScale, const PlacementRange &placementRange,
                bool horizontal, float padding);

private:
    void *cTree;
    void *hTree;
};
}

#endif
