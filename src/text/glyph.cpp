#include <llmr/text/glyph.hpp>

namespace llmr {

// Note: this only works for the BMP
// Note: we could use a binary lookup table to get averaged constant time lookups, however,
// most of our lookups are going to be within the first 3 ranges listed here, so this is
// likely faster.
GlyphRange getGlyphRange(uint32_t glyph) {
    unsigned start = (glyph/256) * 256;
    unsigned end = (start + 255);
    if (start > 65280) start = 65280;
    if (end > 65533) end = 65533;
    return { start, end };
}

}
