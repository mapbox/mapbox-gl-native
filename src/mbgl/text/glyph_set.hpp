#pragma once

#include <mbgl/text/glyph_pbf.hpp>

#include <map>

namespace mbgl {

class GlyphSet {
public:
    void insert(SDFGlyph&&);
    const std::map<uint32_t, SDFGlyph>& getSDFs() const;

private:
    std::map<uint32_t, SDFGlyph> sdfs;
};

} // end namespace mbgl
