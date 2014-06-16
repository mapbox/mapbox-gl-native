#ifndef LLMR_TEXT_PLACEMENT
#define LLMR_TEXT_PLACEMENT

#include <llmr/text/collision.hpp>
#include <llmr/geometry/geometry.hpp>
#include <llmr/util/vec.hpp>
#include <llmr/text/glyph.hpp>
#include <vector>

namespace llmr {

class TextBucket;
class BucketTextDescription;

class Placement {
public:
    Placement(int8_t zoom);

    void addFeature(TextBucket &bucket, const std::vector<Coordinate> &line,
                    const BucketTextDescription &info,
                    const GlyphPositions &face,
                    const Shaping &shaping);

private:
    const int8_t zoom;
    Collision collision;
    const float zOffset;
    const float maxPlacementScale;

public:
    static const int tileExtent;
    static const int glyphSize;
    static const float minScale;
};
}

#endif
