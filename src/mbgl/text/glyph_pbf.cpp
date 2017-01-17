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

namespace mbgl {

namespace {

void parseGlyphPBF(GlyphSet& glyphSet, const GlyphRange& glyphRange, const std::string& data) {
    protozero::pbf_reader glyphs_pbf(data);

    while (glyphs_pbf.next(1)) {
        auto fontstack_pbf = glyphs_pbf.get_message();
        while (fontstack_pbf.next(3)) {
            auto glyph_pbf = fontstack_pbf.get_message();

            SDFGlyph glyph;

            bool hasID = false, hasWidth = false, hasHeight = false, hasLeft = false,
                 hasTop = false, hasAdvance = false;

            while (glyph_pbf.next()) {
                switch (glyph_pbf.tag()) {
                case 1: // id
                    glyph.id = glyph_pbf.get_uint32();
                    hasID = true;
                    break;
                case 2: // bitmap
                    glyph.bitmap = glyph_pbf.get_string();
                    break;
                case 3: // width
                    glyph.metrics.width = glyph_pbf.get_uint32();
                    hasWidth = true;
                    break;
                case 4: // height
                    glyph.metrics.height = glyph_pbf.get_uint32();
                    hasHeight = true;
                    break;
                case 5: // left
                    glyph.metrics.left = glyph_pbf.get_sint32();
                    hasLeft = true;
                    break;
                case 6: // top
                    glyph.metrics.top = glyph_pbf.get_sint32();
                    hasTop = true;
                    break;
                case 7: // advance
                    glyph.metrics.advance = glyph_pbf.get_uint32();
                    hasAdvance = true;
                    break;
                default:
                    glyph_pbf.skip();
                    break;
                }
            }

            // If the area of width/height is non-zero, we need to adjust the expected size
            // with the implicit border size, otherwise we expect there to be no bitmap at all.
            const uint32_t expectedBitmapSize =
                glyph.metrics.width && glyph.metrics.height
                    ? (glyph.metrics.width + 2 * SDFGlyph::borderSize) *
                          (glyph.metrics.height + 2 * SDFGlyph::borderSize)
                    : 0;

            // Only treat this glyph as a correct glyph if it has all required fields, and if
            // the bitmap has the correct length. It also needs to satisfy a few metrics conditions
            // that ensure that the glyph isn't bogus. All other glyphs are malformed.
            // We're also discarding all glyphs that are outside the expected glyph range.
            if (hasID && hasWidth && hasHeight && hasLeft && hasTop && hasAdvance &&
                glyph.metrics.width < 256 && glyph.metrics.height < 256 &&
                glyph.metrics.left >= -128 && glyph.metrics.left < 128 &&
                glyph.metrics.top >= -128 && glyph.metrics.top < 128 &&
                glyph.metrics.advance < 256 && glyph.bitmap.size() == expectedBitmapSize &&
                glyph.id >= glyphRange.first && glyph.id <= glyphRange.second) {
                glyphSet.insert(glyph.id, std::move(glyph));
            }
        }
    }
}

} // namespace

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
                parseGlyphPBF(**atlas->getGlyphSet(fontStack), glyphRange, *res.data);
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
