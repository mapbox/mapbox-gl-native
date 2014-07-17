#ifndef MBGL_TEXT_PLACEMENT
#define MBGL_TEXT_PLACEMENT

#include <mbgl/text/collision.hpp>
#include <mbgl/geometry/geometry.hpp>
#include <mbgl/util/vec.hpp>
#include <mbgl/text/glyph.hpp>
#include <vector>

namespace mbgl {

class TextBucket;
class StyleBucketText;

class Placement {
public:
    Placement(int8_t zoom);

    void addFeature(TextBucket &bucket, const std::vector<Coordinate> &line,
                    const StyleBucketText &info,
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
