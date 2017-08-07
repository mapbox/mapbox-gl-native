#pragma once

#include <utility>
#include <cstdint>
#include <unordered_set>
#include <boost/functional/hash.hpp>

namespace mbgl {

using GlyphRange = std::pair<uint16_t, uint16_t>;

struct GlyphRangeHash {
    std::size_t operator()(const GlyphRange& glyphRange) const {
        return boost::hash_value(glyphRange);
    }
};

using GlyphRangeSet = std::unordered_set<GlyphRange, GlyphRangeHash>;

constexpr uint32_t GLYPHS_PER_GLYPH_RANGE = 256;
constexpr uint32_t GLYPH_RANGES_PER_FONT_STACK = 256;

} // end namespace mbgl
