#include <mbgl/text/local_glyph_rasterizer.hpp>

namespace mbgl {

bool LocalGlyphRasterizer::canRasterizeGlyph(const FontStack&, GlyphID) {
    return false;
}

Glyph LocalGlyphRasterizer::rasterizeGlyph(const FontStack&, GlyphID) {
    return Glyph();
}

} // namespace mbgl
