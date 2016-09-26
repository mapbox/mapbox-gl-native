#pragma once

#include <utility>
#include <cstdint>
#include <unordered_set>
#include <boost/functional/hash.hpp>

namespace mbgl {

typedef std::pair<uint16_t, uint16_t> GlyphRange;

struct GlyphRangeHash {
    std::size_t operator()(const GlyphRange& glyphRange) const {
        return boost::hash_value(glyphRange);
    }
};

typedef std::unordered_set<GlyphRange, GlyphRangeHash> GlyphRangeSet;

constexpr uint32_t GLYPHS_PER_GLYPH_RANGE = 256;
constexpr uint32_t GLYPH_RANGES_PER_FONT_STACK = 256;

} // end namespace mbgl
