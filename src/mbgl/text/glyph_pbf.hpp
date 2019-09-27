#pragma once

#include <mbgl/text/glyph.hpp>
#include <mbgl/text/glyph_range.hpp>

#include <string>
#include <tuple>
#include <vector>

namespace mbgl {

std::tuple<std::vector<Glyph>, int32_t, int32_t> parseGlyphPBF(const GlyphRange&, const std::string& data);

} // namespace mbgl
