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

    SDFGlyph getSDFGlyph(FT_Face face, uint32_t glyphID);
    
    void SetDefaultFont(const std::string& path);
    
    class FontStore : public util::noncopyable {
    public:
        FontStore();
        ~FontStore();
        
        bool UsingDefaultFont() const { return hb_font; }
        FT_Face GetDefaultFT_Face() const { return ft_face; }
        hb_font_t* GetDefaultHB_Font() const { return hb_font; }
        
    private:
        FT_Library library;
        FT_Face ft_face;
        hb_font_t* hb_font;
    };

} // namespace mbgl
