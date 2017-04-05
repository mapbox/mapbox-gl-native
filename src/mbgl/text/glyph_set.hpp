#pragma once

#include <mbgl/text/glyph.hpp>
#include <mbgl/util/geometry.hpp>

namespace mbgl {

class GlyphSet {
public:
    void insert(SDFGlyph&&);
    const std::map<uint32_t, SDFGlyph>& getSDFs() const;

private:
    std::map<uint32_t, SDFGlyph> sdfs;
};

} // end namespace mbgl
