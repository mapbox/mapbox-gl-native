#pragma once

#include <mbgl/text/glyph.hpp>
#include <mbgl/util/geometry.hpp>

namespace mbgl {

class GlyphSet {
public:
    void insert(uint32_t id, SDFGlyph&&);
    const std::map<uint32_t, SDFGlyph>& getSDFs() const;

private:
    std::map<uint32_t, SDFGlyph> sdfs;
};

} // end namespace mbgl
