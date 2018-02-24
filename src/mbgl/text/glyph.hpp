#pragma once

#include <mbgl/text/glyph_range.hpp>
#include <mbgl/util/font_stack.hpp>
#include <mbgl/util/rect.hpp>
#include <mbgl/util/traits.hpp>
#include <mbgl/util/optional.hpp>
#include <mbgl/util/immutable.hpp>
#include <mbgl/util/image.hpp>
#include <mbgl/util/util.hpp>

#include <cstdint>
#include <vector>
#include <string>
#include <map>
#include <set>

namespace mbgl {

using GlyphID = char16_t;
using GlyphIDs = std::set<GlyphID>;
    
// Note: this only works for the BMP
GlyphRange getGlyphRange(GlyphID glyph);

struct GlyphMetrics {
    uint32_t width = 0;
    uint32_t height = 0;
    int32_t left = 0;
    int32_t top = 0;
    uint32_t advance = 0;
};

inline bool operator==(const GlyphMetrics& lhs, const GlyphMetrics& rhs) {
    return lhs.width == rhs.width &&
        lhs.height == rhs.height &&
        lhs.left == rhs.left &&
        lhs.top == rhs.top &&
        lhs.advance == rhs.advance;
}

class Glyph {
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

using Glyphs = std::map<GlyphID, optional<Immutable<Glyph>>>;
using GlyphMap = std::map<FontStack, Glyphs>;

class PositionedGlyph {
public:
    explicit PositionedGlyph(GlyphID glyph_, float x_, float y_, bool vertical_)
        : glyph(glyph_), x(x_), y(y_), vertical(vertical_) {}

    GlyphID glyph = 0;
    float x = 0;
    float y = 0;
    bool vertical = false;
};

enum class WritingModeType : uint8_t;

class Shaping {
    public:
    explicit Shaping() = default;
    explicit Shaping(float x, float y, WritingModeType writingMode_)
        : top(y), bottom(y), left(x), right(x), writingMode(writingMode_) {}
    std::vector<PositionedGlyph> positionedGlyphs;
    float top = 0;
    float bottom = 0;
    float left = 0;
    float right = 0;
    WritingModeType writingMode;

    explicit operator bool() const { return !positionedGlyphs.empty(); }
};

enum class WritingModeType : uint8_t {
    None = 0,
    Horizontal = 1 << 0,
    Vertical = 1 << 1,
};

MBGL_CONSTEXPR WritingModeType operator|(WritingModeType a, WritingModeType b) {
    return WritingModeType(mbgl::underlying_type(a) | mbgl::underlying_type(b));
}

MBGL_CONSTEXPR WritingModeType& operator|=(WritingModeType& a, WritingModeType b) {
    return (a = a | b);
}

MBGL_CONSTEXPR bool operator&(WritingModeType lhs, WritingModeType rhs) {
    return mbgl::underlying_type(lhs) & mbgl::underlying_type(rhs);
}

MBGL_CONSTEXPR WritingModeType& operator&=(WritingModeType& lhs, WritingModeType rhs) {
    return (lhs = WritingModeType(mbgl::underlying_type(lhs) & mbgl::underlying_type(rhs)));
}

MBGL_CONSTEXPR WritingModeType operator~(WritingModeType value) {
    return WritingModeType(~mbgl::underlying_type(value));
}

using GlyphDependencies = std::map<FontStack,GlyphIDs>;
using GlyphRangeDependencies = std::map<FontStack,GlyphRangeSet>;

} // end namespace mbgl
