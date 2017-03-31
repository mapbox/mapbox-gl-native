#include <mbgl/text/glyph.hpp>

namespace mbgl {

// Note: this only works for the BMP
GlyphRange getGlyphRange(GlyphID glyph) {
    unsigned start = (glyph/256) * 256;
    unsigned end = (start + 255);
    if (start > 65280) start = 65280;
    if (end > 65535) end = 65535;
    return { start, end };
}

} // namespace mbgl
