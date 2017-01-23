#pragma once

#include <mbgl/text/bidi.hpp>
#include <mbgl/text/glyph.hpp>
#include <mbgl/util/geometry.hpp>

namespace mbgl {

class GlyphSet {
public:
    void insert(uint32_t id, SDFGlyph&&);
    const std::map<uint32_t, SDFGlyph>& getSDFs() const;
    const Shaping getShaping(const std::u16string& string,
                             float maxWidth,
                             float lineHeight,
                             float horizontalAlign,
                             float verticalAlign,
                             float justify,
                             float spacing,
                             const Point<float>& translate,
                             BiDi& bidi) const;

private:
    float determineAverageLineWidth(const std::u16string& logicalInput,
                                        const float spacing,
                                        float maxWidth) const;
    std::set<std::size_t> determineLineBreaks(const std::u16string& logicalInput,
                                          const float spacing,
                                          float maxWidth) const;

    void shapeLines(Shaping& shaping,
                    const std::vector<std::u16string>& lines,
                    const float spacing,
                    float lineHeight,
                    float horizontalAlign,
                    float verticalAlign,
                    float justify,
                    const Point<float>& translate) const;

    std::map<uint32_t, SDFGlyph> sdfs;
};

} // end namespace mbgl
