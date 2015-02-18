#ifndef MBGL_TEXT_PLACEMENT
#define MBGL_TEXT_PLACEMENT

#include <mbgl/text/types.hpp>
#include <mbgl/text/glyph.hpp>

#include <mbgl/util/vec.hpp>

namespace mbgl {

struct Anchor;
class StyleLayoutSymbol;

class Placement {
public:
    static Placement getIcon(Anchor &anchor, const Rect<uint16_t> &image, float iconBoxScale,
                             const std::vector<Coordinate> &line, const StyleLayoutSymbol &layout);

    static Placement getGlyphs(Anchor &anchor, const vec2<float> &origin, const Shaping &shaping,
                               const GlyphPositions &face, float boxScale, bool horizontal,
                               const std::vector<Coordinate> &line, const StyleLayoutSymbol &layout);

    static const float globalMinScale;

    GlyphBoxes boxes;
    PlacedGlyphs shapes;
    float minScale;
};
}

#endif
