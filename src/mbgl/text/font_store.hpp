#pragma once

#include <mbgl/text/glyph.hpp>
#include <mbgl/util/noncopyable.hpp>

// freetype2
extern "C"
{
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H
#include FT_OUTLINE_H
}

struct hb_font_t;

namespace mbgl {

    struct LocalFont {
        LocalFont(uint32_t id_, FT_Face ft_face_, hb_font_t* hb_font_)
            : id(id_), ft_face(ft_face_), hb_font(hb_font_)
        {}
        
        uint32_t id;
        FT_Face ft_face;
        hb_font_t* hb_font;
    };
    typedef std::vector<LocalFont> LocalFonts;
    typedef std::set<uint32_t> RequiredGlyphs;
    typedef std::pair<uint32_t, RequiredGlyphs> RequiredGlyphsForFont;
    typedef std::map<uint32_t, RequiredGlyphs> RequiredGlyphFontMap;
    

    SDFGlyph getSDFGlyph(FT_Face face, uint32_t glyphID);
    
    class FontStore : public util::noncopyable {
    public:
        FontStore();
        ~FontStore();
        
        bool UsingLocalFonts() const { return !localFonts.empty(); }
        const std::vector<LocalFont>& GetLocalFonts() const { return localFonts; }
        FT_Face GetFT_Face(uint32_t font_id) const { return localFonts[font_id].ft_face; } // TODO: Safe access!
        hb_font_t* GetHB_Font(uint32_t font_id) const { return localFonts[font_id].hb_font; }
        
    private:
        FT_Library library;
        std::vector<LocalFont> localFonts;
    };

} // namespace mbgl
