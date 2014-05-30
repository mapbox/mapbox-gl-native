#ifndef LLMR_TEXT_GLYPH
#define LLMR_TEXT_GLYPH

#include <llmr/util/rect.hpp>

#include <cstdint>
#include <vector>
#include <map>

namespace llmr {

typedef std::pair<uint16_t, uint16_t> GlyphRange;

// Note: this only works for the BMP
GlyphRange getGlyphRange(uint32_t glyph);

struct GlyphMetrics {
    operator bool() const {
        return width == 0 && height == 0 && advance == 0;
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
    inline explicit Glyph(const Rect<uint16_t> &rect,
                          const GlyphMetrics &metrics)
        : rect(rect), metrics(metrics) {}

    operator bool() const {
        return !metrics && !rect;
    }

    const Rect<uint16_t> rect;
    const GlyphMetrics metrics;
};

typedef std::map<uint32_t, Glyph> GlyphPositions;
typedef std::map<std::string, GlyphPositions> Faces;
typedef std::vector<const GlyphPositions *> IndexedFaces;

class GlyphPlacement {
public:
    inline explicit GlyphPlacement(uint32_t face, uint32_t glyph, uint32_t x, uint32_t y)
        : face(face), glyph(glyph), x(x), y(y) {}

    uint32_t face = 0;
    uint32_t glyph = 0;
    int32_t x = 0;
    int32_t y = 0;
};

typedef std::vector<GlyphPlacement> Shaping;
}

#endif
