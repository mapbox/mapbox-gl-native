#pragma once

#include <mbgl/text/glyph.hpp>
#include <mbgl/text/glyph_range.hpp>

#include <string>
#include <utility>
#include <vector>

namespace mbgl {

std::pair<std::vector<Glyph>, bool> parseGlyphPBF(const GlyphRange&, const std::string& data);

} // namespace mbgl
