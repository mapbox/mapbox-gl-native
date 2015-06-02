#ifndef MBGL_TEXT_FONT_STACK
#define MBGL_TEXT_FONT_STACK

#include <mbgl/text/glyph.hpp>
#include <mbgl/util/vec.hpp>

namespace mbgl {

class FontStack {
public:
    void insert(uint32_t id, const SDFGlyph &glyph);
    const std::map<uint32_t, GlyphMetrics> &getMetrics() const;
    const std::map<uint32_t, SDFGlyph> &getSDFs() const;
    const Shaping getShaping(const std::u32string &string, float maxWidth, float lineHeight,
                             float horizontalAlign, float verticalAlign, float justify,
                             float spacing, const vec2<float> &translate) const;
    void lineWrap(Shaping &shaping, float lineHeight, float maxWidth, float horizontalAlign,
                  float verticalAlign, float justify) const;

private:
    std::map<uint32_t, std::string> bitmaps;
    std::map<uint32_t, GlyphMetrics> metrics;
    std::map<uint32_t, SDFGlyph> sdfs;
};

} // end namespace mbgl

#endif
