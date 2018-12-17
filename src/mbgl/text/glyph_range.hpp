#pragma once

#include <utility>
#include <cstdint>
#include <unordered_set>
#include <mbgl/util/hash.hpp>

namespace mbgl {

using GlyphRange = std::pair<uint16_t, uint16_t>;

constexpr uint32_t GLYPHS_PER_GLYPH_RANGE = 256;
constexpr uint32_t GLYPH_RANGES_PER_FONT_STACK = 256;

} // end namespace mbgl

namespace std {

template <>
struct hash<mbgl::GlyphRange> {
    std::size_t operator()(const mbgl::GlyphRange& range) const {
        return mbgl::util::hash(range.first, range.second);
    }
};

} // namespace std
