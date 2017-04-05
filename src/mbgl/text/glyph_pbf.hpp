#pragma once

#include <mbgl/text/glyph.hpp>
#include <mbgl/text/glyph_range.hpp>
#include <mbgl/util/image.hpp>

#include <string>
#include <vector>

namespace mbgl {

class SDFGlyph {
public:
    // We're using this value throughout the Mapbox GL ecosystem. If this is different, the glyphs
    // also need to be reencoded.
    static constexpr const uint8_t borderSize = 3;

    GlyphID id = 0;

    // A signed distance field of the glyph with a border (see above).
    AlphaImage bitmap;

    // Glyph metrics
    GlyphMetrics metrics;
};

std::vector<SDFGlyph> parseGlyphPBF(const GlyphRange&, const std::string& data);

} // namespace mbgl
