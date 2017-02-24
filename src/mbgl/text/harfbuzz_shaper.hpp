#pragma once

#include <set>
#include <string>

#include <mbgl/text/glyph.hpp>
#include <mbgl/text/font_store.hpp>

struct hb_font_t;

namespace mbgl {
namespace harfbuzz {

RequiredGlyphsForFont getGlyphIDs(const LocalFonts& fonts, const std::string& u8label);
    
std::vector<std::pair<char16_t,double>> getClusterWidths(hb_font_t* font, const std::u16string& u16text);
void applyShaping(hb_font_t* font, const std::u16string& u16text, std::vector<PositionedGlyph>& positionedGlyphs, float& current_x, float& current_y);
    
}
}
