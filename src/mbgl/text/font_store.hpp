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
        LocalFont(uint32_t id_, const std::string& path);
        ~LocalFont();
        
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
        
        bool UsingLocalFonts() const;
        const std::vector<LocalFont>& GetLocalFonts() const;
        FT_Face GetFT_Face(uint32_t font_id) const;
        hb_font_t* GetHB_Font(uint32_t font_id) const;
    };

} // namespace mbgl
