#pragma once

#include <mbgl/text/glyph_range.hpp>
#include <mbgl/util/font_stack.hpp>
#include <mbgl/util/rect.hpp>
#include <mbgl/util/traits.hpp>
#include <mbgl/util/optional.hpp>

#include <cstdint>
#include <vector>
#include <string>
#include <map>

namespace mbgl {

typedef char16_t GlyphID;
typedef std::set<GlyphID> GlyphIDs;
    
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

struct Glyph {
    Rect<uint16_t> rect;
    GlyphMetrics metrics;
};

typedef std::map<GlyphID, optional<Glyph>> GlyphPositions;
typedef std::map<FontStack, GlyphPositions> GlyphPositionMap;

class PositionedGlyph {
public:
    explicit PositionedGlyph(GlyphID glyph_, float x_, float y_, float angle_)
        : glyph(glyph_), x(x_), y(y_), angle(angle_) {}

    GlyphID glyph = 0;
    float x = 0;
    float y = 0;
    float angle = 0;
};

enum class WritingModeType : uint8_t;

class Shaping {
    public:
    explicit Shaping() : top(0), bottom(0), left(0), right(0) {}
    explicit Shaping(float x, float y, WritingModeType writingMode_)
        : top(y), bottom(y), left(x), right(x), writingMode(writingMode_) {}
    std::vector<PositionedGlyph> positionedGlyphs;
    int32_t top;
    int32_t bottom;
    int32_t left;
    int32_t right;
    WritingModeType writingMode;

    explicit operator bool() const { return !positionedGlyphs.empty(); }
};

enum class WritingModeType : uint8_t {
    None = 0,
    Horizontal = 1 << 0,
    Vertical = 1 << 1,
};

constexpr WritingModeType operator|(WritingModeType a, WritingModeType b) {
    return WritingModeType(mbgl::underlying_type(a) | mbgl::underlying_type(b));
}

constexpr WritingModeType& operator|=(WritingModeType& a, WritingModeType b) {
    return (a = a | b);
}

constexpr bool operator&(WritingModeType lhs, WritingModeType rhs) {
    return mbgl::underlying_type(lhs) & mbgl::underlying_type(rhs);
}

constexpr WritingModeType& operator&=(WritingModeType& lhs, WritingModeType rhs) {
    return (lhs = WritingModeType(mbgl::underlying_type(lhs) & mbgl::underlying_type(rhs)));
}

constexpr WritingModeType operator~(WritingModeType value) {
    return WritingModeType(~mbgl::underlying_type(value));
}

typedef std::map<FontStack,GlyphIDs> GlyphDependencies;
typedef std::map<FontStack,GlyphRangeSet> GlyphRangeDependencies;


} // end namespace mbgl
