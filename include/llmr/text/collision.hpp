#ifndef LLMR_TEXT_COLLISION
#define LLMR_TEXT_COLLISION

#include <llmr/text/types.hpp>

namespace llmr {

class Collision {

public:
    Collision();
    ~Collision();

    PlacementProperty place(const PlacedGlyphs &boxes,
                            const CollisionAnchor &anchor,
                            float minPlacementScale, float maxPlacementScale,
                            float padding, bool horizontal, bool alwaysVisible);
    float getPlacementScale(const GlyphBoxes &glyphs, float minPlacementScale,
                            float maxPlacementScale, float pad);
    PlacementRange getPlacementRange(const GlyphBoxes &glyphs,
                                     float placementScale);
    void insert(const GlyphBoxes &glyphs, const CollisionAnchor &anchor,
                float placementScale, const PlacementRange &placementRange,
                bool horizontal, float padding);

private:
    void *tree;
};
}

#endif
