#ifndef MBGL_TEXT_GLYPH
#define MBGL_TEXT_GLYPH

#include <mbgl/util/rect.hpp>

#include <cstdint>
#include <vector>
#include <string>
#include <map>

namespace mbgl {

typedef std::pair<uint16_t, uint16_t> GlyphRange;

// Note: this only works for the BMP
GlyphRange getGlyphRange(char32_t glyph);

struct GlyphMetrics {
    operator bool() const {
        return !(width == 0 && height == 0 && advance == 0);
    }

    // Glyph metrics.
    uint32_t width = 0;
    uint32_t height = 0;
    int32_t left = 0;
    int32_t top = 0;
    uint32_t advance = 0;

};

struct Glyph {
    inline explicit Glyph() : rect(0, 0, 0, 0), metrics() {}
    inline explicit Glyph(const Rect<uint16_t> &rect_,
                          const GlyphMetrics &metrics_)
        : rect(rect_), metrics(metrics_) {}

    operator bool() const {
        return metrics || rect.hasArea();
    }

    const Rect<uint16_t> rect;
    const GlyphMetrics metrics;
};

typedef std::map<uint32_t, Glyph> GlyphPositions;

class PositionedGlyph {
public:
    inline explicit PositionedGlyph(uint32_t glyph_, float x_, float y_)
        : glyph(glyph_), x(x_), y(y_) {}

    uint32_t glyph = 0;
    float x = 0;
    float y = 0;
};

class Shaping {
    public:
    inline explicit Shaping() : top(0), bottom(0), left(0), right(0) {}
    inline explicit Shaping(float x, float y, std::u32string text_)
        : text(text_), top(y), bottom(y), left(x), right(x) {}
    std::vector<PositionedGlyph> positionedGlyphs;
    std::u32string text;
    int32_t top;
    int32_t bottom;
    int32_t left;
    int32_t right;

    operator bool() const { return !positionedGlyphs.empty(); }
};

class SDFGlyph {
public:
    uint32_t id = 0;

    // A signed distance field of the glyph with a border of 3 pixels.
    std::string bitmap;

    // Glyph metrics
    GlyphMetrics metrics;
};

} // end namespace mbgl

#endif
