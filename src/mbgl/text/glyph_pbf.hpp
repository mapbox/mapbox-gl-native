#pragma once

#include <mbgl/text/glyph.hpp>
#include <mbgl/text/glyph_range.hpp>

#include <string>
#include <vector>

namespace mbgl {

std::vector<Glyph> parseGlyphPBF(const GlyphRange&, const std::string& data);

} // namespace mbgl
