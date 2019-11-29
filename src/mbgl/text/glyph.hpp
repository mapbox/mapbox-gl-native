#pragma once

#include <mbgl/text/glyph_range.hpp>
#include <mbgl/util/bitmask_operations.hpp>
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
    uint32_t width = 0U;
    uint32_t height = 0U;
    int32_t left = 0;
    int32_t top = 0;
    uint32_t advance = 0U;
};

inline bool operator==(const GlyphMetrics& lhs, const GlyphMetrics& rhs) {
    return lhs.width == rhs.width && lhs.height == rhs.height && lhs.left == rhs.left && lhs.top == rhs.top &&
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

struct Glyphs {
    std::map<GlyphID, optional<Immutable<Glyph>>> glyphs{};
    optional<int32_t> ascender{nullopt};
    optional<int32_t> descender{nullopt};
};
using GlyphMap = std::map<FontStackHash, Glyphs>;

class PositionedGlyph {
public:
    explicit PositionedGlyph(GlyphID glyph_, float x_, float y_, bool vertical_, FontStackHash font_, float scale_, std::size_t sectionIndex_ = 0)
        : glyph(glyph_), x(x_), y(y_), vertical(vertical_), font(font_), scale(scale_), sectionIndex(sectionIndex_)
    {}

    GlyphID glyph = 0;
    float x = 0;
    float y = 0;
    bool vertical = false;
    
    FontStackHash font = 0;
    float scale = 0.0;
    // Maps positioned glyph to TaggedString section
    std::size_t sectionIndex;
};

enum class WritingModeType : uint8_t;

class Shaping {
    public:
    Shaping() = default;
    explicit Shaping(float x, float y, WritingModeType writingMode_, std::size_t lineCount_)
        : top(y), bottom(y), left(x), right(x), writingMode(writingMode_), lineCount(lineCount_) {}
    std::unordered_map<uint32_t, std::vector<PositionedGlyph>> positionedGlyphs;
    float top = 0;
    float bottom = 0;
    float left = 0;
    float right = 0;
    WritingModeType writingMode;
    std::size_t lineCount = 0u;
    explicit operator bool() const { return !positionedGlyphs.empty(); }
    // The y offset *should* be part of the font metadata.
    static constexpr int32_t yOffset = -17;
    bool hasBaseline{false};
};

enum class WritingModeType : uint8_t {
    None = 0,
    Horizontal = 1 << 0,
    Vertical = 1 << 1,
};

using GlyphDependencies = std::map<FontStack, GlyphIDs>;
using GlyphRangeDependencies = std::map<FontStack, std::unordered_set<GlyphRange>>;

} // end namespace mbgl
