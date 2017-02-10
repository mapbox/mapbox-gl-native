#include <mbgl/text/harfbuzz_shaper.hpp>

#include <harfbuzz/hb.h>
#include <harfbuzz/hb-ft.h>

// freetype2
extern "C"
{
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H
#include FT_OUTLINE_H
}

namespace mbgl {
namespace harfbuzz {
    
RequiredGlyphs getGlyphIDs(hb_font_t* font, const std::string& u8label) {
    hb_buffer_t *hb_buffer;
    hb_buffer = hb_buffer_create();
    hb_buffer_add_utf8(hb_buffer, u8label.c_str(), -1, 0, -1);
    hb_buffer_guess_segment_properties(hb_buffer);
    
    hb_shape(font, hb_buffer, NULL, 0);
    
    /* Get glyph information and positions out of the buffer. */
    unsigned int len = hb_buffer_get_length(hb_buffer);
    hb_glyph_info_t *glyphs = hb_buffer_get_glyph_infos(hb_buffer, NULL);
    
    RequiredGlyphs requiredGlyphs;
    for (uint32_t i = 0; i < len; i++)
    {
        // "codepoint" means "glyph id" after shaping
        if (!glyphs[i].codepoint) {
            hb_buffer_destroy(hb_buffer);
            return RequiredGlyphs(); // There's an invalid glyph in here, fall back to another font
        }
        requiredGlyphs.insert(glyphs[i].codepoint);
    }
    hb_buffer_destroy(hb_buffer);
    return requiredGlyphs;
}
    
RequiredGlyphsForFont getGlyphIDs(const LocalFonts& localFonts, const std::string& u8label) {
    for (auto localFont : localFonts) {
        RequiredGlyphs glyphs = getGlyphIDs(localFont.hb_font, u8label);
        if (!glyphs.empty())
            return RequiredGlyphsForFont(localFont.id,glyphs);
    }
    return RequiredGlyphsForFont(localFonts.back().id, RequiredGlyphs()); // All tofu for this label
}

void applyShaping(hb_font_t* font, const std::string& u8text, std::vector<PositionedGlyph>& positionedGlyphs, float& current_x, float& current_y) {
    hb_buffer_t *hb_buffer;
    hb_buffer = hb_buffer_create ();
    
    hb_buffer_add_utf8(hb_buffer, u8text.c_str(), -1, 0, -1);
    hb_buffer_guess_segment_properties(hb_buffer);
    hb_buffer_set_direction(hb_buffer, HB_DIRECTION_LTR); // We set the direction LTR because by the time we get here we've already reversed RTL text with BiDi
    
    hb_shape(font, hb_buffer, NULL, 0);
    
    /* Get glyph information and positions out of the buffer. */
    unsigned int len = hb_buffer_get_length(hb_buffer);
    hb_glyph_info_t *info = hb_buffer_get_glyph_infos(hb_buffer, NULL);
    hb_glyph_position_t *pos = hb_buffer_get_glyph_positions(hb_buffer, NULL);
    
    for (unsigned int i = 0; i < len; i++)
    {
        hb_codepoint_t gid   = info[i].codepoint; // "codepoint" means "glyph id" after shaping
        //unsigned int cluster = info[i].cluster;
        double x_advance = pos[i].x_advance / 64.;
        double y_advance = pos[i].y_advance / 64.;
        double x_offset  = pos[i].x_offset / 64.;
        double y_offset  = pos[i].y_offset / 64.;
        
        
        positionedGlyphs.emplace_back(gid, current_x + x_offset, current_y + y_offset);
        current_x += x_advance;
        current_y += y_advance;
    }
    
    hb_buffer_destroy(hb_buffer);
}

}
}
