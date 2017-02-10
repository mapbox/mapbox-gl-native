#include <mbgl/text/font_store.hpp>
#include <mbgl/text/default_font.hpp>
#include <mbgl/util/thread_local.hpp>

#include <harfbuzz/hb-ft.h>

#include <mapbox/glyph_foundry.hpp>
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wshadow"
#pragma GCC diagnostic ignored "-Wunused-local-typedef"
#pragma GCC diagnostic ignored "-Wunused-parameter"
#include <mapbox/glyph_foundry_impl.hpp>
#pragma GCC diagnostic pop


namespace mbgl {

    static util::ThreadLocal<FT_Library> library;
    static util::ThreadLocal<std::vector<LocalFont>> localFonts; // TODO: Shouldn't really be global...
    
    LocalFont::LocalFont(uint32_t id_, const std::string& path)
        : id(id_), ft_face(0)
    {
        if (!library.get()) {
            FT_Library* library_init = new FT_Library;
            // TODO: Match with FT_Done_FreeType(library);
            FT_Error error = FT_Init_FreeType(library_init);
            if (error) {
                throw std::runtime_error("could not open FreeType library");
            }
            library.set(library_init);
        }
        FT_Error face_error = FT_New_Face(*library.get(), path.c_str(), 0, &ft_face);
        if (face_error) {
            throw std::runtime_error("could not open font");
        }
        
        const double scale_factor = 1.0;
        double size = 24 * scale_factor;
        FT_Set_Char_Size(ft_face,0,(FT_F26Dot6)(size * (1<<6)),0,0);
        
        hb_font = hb_ft_font_create(ft_face, NULL);
    }
    
    LocalFont::~LocalFont() {
        // Not being used yet since LocalFonts are just held in a static ThreadLocal
        /*if (hb_font) {
            hb_font_destroy(hb_font);
        }
        if (ft_face) {
            FT_Done_Face(ft_face);
        }*/
    }

    FontStore::FontStore() {

    }

    bool FontStore::UsingLocalFonts() const {
        return !GetDefaultFontPaths().empty();
    }
    
    
    const std::vector<LocalFont>& FontStore::GetLocalFonts() const {
        if (!localFonts.get()) {
            uint32_t fontID = 0;
            std::vector<LocalFont>* threadLocalFonts = new std::vector<LocalFont>;
            for (auto path : GetDefaultFontPaths()) {
                threadLocalFonts->emplace_back(fontID++, path);
            }
            localFonts.set(threadLocalFonts);
        }
        return *localFonts.get();
    }
    
    FT_Face FontStore::GetFT_Face(uint32_t font_id) const {
        if (!localFonts.get()) {
            throw std::runtime_error("FontStore not initialized");
        }
        const std::vector<LocalFont>& threadLocalFonts = *localFonts.get();
        return threadLocalFonts[font_id].ft_face;
    }
    
    hb_font_t* FontStore::GetHB_Font(uint32_t font_id) const {
        if (!localFonts.get()) {
            throw std::runtime_error("FontStore not initialized");
        }
        const std::vector<LocalFont>& threadLocalFonts = *localFonts.get();
        return threadLocalFonts[font_id].hb_font;
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
