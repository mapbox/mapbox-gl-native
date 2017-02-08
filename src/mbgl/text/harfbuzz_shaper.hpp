#pragma once

#include <set>
#include <string>

#include <mbgl/text/glyph.hpp>

struct hb_font_t;

namespace mbgl {
namespace harfbuzz {

std::set<uint32_t> getGlyphIDs(hb_font_t* font, const std::string& u8label);
    
void applyShaping(hb_font_t* font, const std::string& u8text, std::vector<PositionedGlyph>& positionedGlyphs, float& current_x, float& current_y);
    
}
}
