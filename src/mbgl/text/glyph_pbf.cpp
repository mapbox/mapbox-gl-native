#include <mbgl/text/glyph_pbf.hpp>
#include <mbgl/text/glyph_atlas.hpp>
#include <mbgl/text/glyph_atlas_observer.hpp>
#include <mbgl/text/glyph_set.hpp>
#include <mbgl/storage/file_source.hpp>
#include <mbgl/storage/resource.hpp>
#include <mbgl/storage/response.hpp>
#include <mbgl/util/exception.hpp>
#include <mbgl/util/string.hpp>
#include <mbgl/util/token.hpp>
#include <mbgl/util/url.hpp>

#include <protozero/pbf_reader.hpp>

namespace {

void parseGlyphPBF(mbgl::GlyphSet& glyphSet, const std::string& data) {
    protozero::pbf_reader glyphs_pbf(data);

    while (glyphs_pbf.next(1)) {
        auto fontstack_pbf = glyphs_pbf.get_message();
        while (fontstack_pbf.next(3)) {
            auto glyph_pbf = fontstack_pbf.get_message();

            mbgl::SDFGlyph glyph;

            while (glyph_pbf.next()) {
                switch (glyph_pbf.tag()) {
                case 1: // id
                    glyph.id = glyph_pbf.get_uint32();
                    break;
                case 2: // bitmap
                    glyph.bitmap = glyph_pbf.get_string();
                    break;
                case 3: // width
                    glyph.metrics.width = glyph_pbf.get_uint32();
                    break;
                case 4: // height
                    glyph.metrics.height = glyph_pbf.get_uint32();
                    break;
                case 5: // left
                    glyph.metrics.left = glyph_pbf.get_sint32();
                    break;
                case 6: // top
                    glyph.metrics.top = glyph_pbf.get_sint32();
                    break;
                case 7: // advance
                    glyph.metrics.advance = glyph_pbf.get_uint32();
                    break;
                default:
                    glyph_pbf.skip();
                    break;
                }
            }

            glyphSet.insert(glyph.id, std::move(glyph));
        }
    }
}

} // namespace

namespace mbgl {

GlyphPBF::GlyphPBF(GlyphAtlas* atlas,
                   const FontStack& fontStack,
                   const GlyphRange& glyphRange,
                   GlyphAtlasObserver* observer_,
                   FileSource& fileSource)
    : parsed(false),
      observer(observer_) {
    req = fileSource.request(Resource::glyphs(atlas->getURL(), fontStack, glyphRange), [this, atlas, fontStack, glyphRange](Response res) {
        if (res.error) {
            observer->onGlyphsError(fontStack, glyphRange, std::make_exception_ptr(std::runtime_error(res.error->message)));
        } else if (res.notModified) {
            return;
        } else if (res.noContent) {
            parsed = true;
            observer->onGlyphsLoaded(fontStack, glyphRange);
        } else {
            try {
                parseGlyphPBF(**atlas->getGlyphSet(fontStack), *res.data);
            } catch (...) {
                observer->onGlyphsError(fontStack, glyphRange, std::current_exception());
                return;
            }

            parsed = true;
            observer->onGlyphsLoaded(fontStack, glyphRange);
        }
    });
}

GlyphPBF::~GlyphPBF() = default;

} // namespace mbgl
