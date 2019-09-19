#include <mbgl/text/glyph_pbf.hpp>

#include <protozero/pbf_reader.hpp>

namespace mbgl {

std::vector<Glyph> parseGlyphPBF(const GlyphRange& glyphRange, const std::string& data) {
    std::vector<Glyph> result;
    result.reserve(256);

    protozero::pbf_reader glyphs_pbf(data);

    while (glyphs_pbf.next(1)) {
        auto readGlyphMetrics = [glyphRange, &result](protozero::pbf_reader& fontstack_pbf) {
            auto glyph_pbf = fontstack_pbf.get_message();
            Glyph glyph;
            protozero::data_view glyphData;

            bool hasID = false, hasWidth = false, hasHeight = false, hasLeft = false,
                 hasTop = false, hasAdvance = false;

            while (glyph_pbf.next()) {
                switch (glyph_pbf.tag()) {
                case 1: // id
                    glyph.id = glyph_pbf.get_uint32();
                    hasID = true;
                    break;
                case 2: // bitmap
                    glyphData = glyph_pbf.get_view();
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

            // Only treat this glyph as a correct glyph if it has all required fields. It also
            // needs to satisfy a few metrics conditions that ensure that the glyph isn't bogus.
            // All other glyphs are malformed.  We're also discarding all glyphs that are outside
            // the expected glyph range.
            if (!hasID || !hasWidth || !hasHeight || !hasLeft || !hasTop || !hasAdvance ||
                glyph.metrics.width >= 256 || glyph.metrics.height >= 256 ||
                glyph.metrics.left < -128 || glyph.metrics.left >= 128 ||
                glyph.metrics.top < -128 || glyph.metrics.top >= 128 ||
                glyph.metrics.advance >= 256 || glyph.id < glyphRange.first ||
                glyph.id > glyphRange.second) {
                return;
            }

            // If the area of width/height is non-zero, we need to adjust the expected size
            // with the implicit border size, otherwise we expect there to be no bitmap at all.
            if (glyph.metrics.width && glyph.metrics.height) {
                const Size size{ glyph.metrics.width + 2 * Glyph::borderSize,
                                 glyph.metrics.height + 2 * Glyph::borderSize };

                if (size.area() != glyphData.size()) {
                    return;
                }

                glyph.bitmap = AlphaImage(size, reinterpret_cast<const uint8_t*>(glyphData.data()),
                                          glyphData.size());
            }

            result.push_back(std::move(glyph));
        };

        double ascender{ 0.0 }, descender{ 0.0 };
        uint16_t count{ 0 };
        auto fontstack_pbf = glyphs_pbf.get_message();
        while (fontstack_pbf.next()) {
            switch (fontstack_pbf.tag()) {
            case 3: {
                readGlyphMetrics(fontstack_pbf);
                ++count;
                break;
            }
            case 4: {
                ascender = fontstack_pbf.get_double();
                break;
            }
            case 5: {
                descender = fontstack_pbf.get_double();
                break;
            }
            default: {
                fontstack_pbf.skip();
                break;
            }
            }
        }
        if (ascender != 0.0 || descender != 0.0) {
            assert(count <= result.size());
            for (uint16_t i = result.size() - count; i <= result.size() - 1; ++i) {
                result[i].metrics.ascender = ascender;
                result[i].metrics.descender = descender;
            }
        }
    }

    return result;
}

} // namespace mbgl
