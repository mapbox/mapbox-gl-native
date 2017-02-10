#pragma once

#include <set>
#include <string>

#include <mbgl/text/glyph.hpp>
#include <mbgl/text/font_store.hpp>

struct hb_font_t;

namespace mbgl {
namespace harfbuzz {

RequiredGlyphsForFont getGlyphIDs(const LocalFonts& fonts, const std::string& u8label);
    
void applyShaping(hb_font_t* font, const std::string& u8text, std::vector<PositionedGlyph>& positionedGlyphs, float& current_x, float& current_y);
    
}
}
