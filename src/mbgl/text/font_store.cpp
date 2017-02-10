#include <mbgl/text/font_store.hpp>
#include <mbgl/text/default_font.hpp>

#include <harfbuzz/hb-ft.h>

#include <mapbox/glyph_foundry.hpp>
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wshadow"
#pragma GCC diagnostic ignored "-Wunused-local-typedef"
#pragma GCC diagnostic ignored "-Wunused-parameter"
#include <mapbox/glyph_foundry_impl.hpp>
#pragma GCC diagnostic pop


namespace mbgl {


    FontStore::FontStore() : library(nullptr) {
        if (GetDefaultFontPaths().empty()) {
            return;
        }
        FT_Error error = FT_Init_FreeType(&library);
        if (error) {
            throw std::runtime_error("could not open FreeType library");
        }
        uint32_t fontID = 0;
        for (auto path : GetDefaultFontPaths()) {
            FT_Face ft_face = 0;
            
            FT_Error face_error = FT_New_Face(library, path.c_str(), 0, &ft_face);
            if (face_error) {
                throw std::runtime_error("could not open font");
            }

            const double scale_factor = 1.0;
            double size = 24 * scale_factor;
            FT_Set_Char_Size(ft_face,0,(FT_F26Dot6)(size * (1<<6)),0,0);
            
            hb_font_t* hb_font = hb_ft_font_create(ft_face, NULL);

            localFonts.emplace_back(fontID++, ft_face, hb_font);
        }
    }

    FontStore::~FontStore() {
        for (auto font : localFonts) {
            if (font.hb_font) {
                hb_font_destroy(font.hb_font);
            }
            if (font.ft_face) {
                FT_Done_Face(font.ft_face);
            }
        }
        if (library) {
            FT_Done_FreeType(library);
        }
    }

    SDFGlyph getSDFGlyph(FT_Face face, uint32_t glyphID) {
        sdf_glyph_foundry::glyph_info glyph;
        glyph.glyph_index = glyphID;
        sdf_glyph_foundry::RenderSDF(glyph, 24, 3, 0.25, face);

        SDFGlyph result;
        GlyphMetrics metrics;
        result.id = glyphID;
        result.bitmap = glyph.bitmap;

        metrics.width = glyph.width;
        metrics.height = glyph.height;
        metrics.left = glyph.left;
        metrics.top = glyph.top;
        metrics.advance = glyph.advance;

        result.metrics = metrics;
        return result;
    }



} // namespace mbgl
