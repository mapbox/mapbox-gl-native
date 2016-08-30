#pragma once

#include <mbgl/text/glyph.hpp>
#include <mbgl/util/geometry.hpp>

namespace mbgl {

class GlyphSet {
public:
    void insert(uint32_t id, SDFGlyph&&);
    const std::map<uint32_t, SDFGlyph> &getSDFs() const;
    const Shaping getShaping(const std::u32string &string, float maxWidth, float lineHeight,
                             float horizontalAlign, float verticalAlign, float justify,
                             float spacing, const Point<float> &translate) const;
    void lineWrap(Shaping &shaping, float lineHeight, float maxWidth, float horizontalAlign,
                  float verticalAlign, float justify, const Point<float> &translate) const;

private:
    std::map<uint32_t, SDFGlyph> sdfs;
};

} // end namespace mbgl
