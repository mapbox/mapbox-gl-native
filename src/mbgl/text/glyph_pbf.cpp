#include <mbgl/text/glyph_pbf.hpp>

#include <protozero/pbf_reader.hpp>

namespace mbgl {

std::tuple<std::vector<Glyph>, int32_t, int32_t> parseGlyphPBF(const GlyphRange& glyphRange, const std::string& data) {
    std::vector<Glyph> glyphs;
    glyphs.reserve(256);
    int32_t ascender{0}, descender{0};
    bool ascenderSet{false}, descenderSet{false};

    protozero::pbf_reader glyphs_pbf(data);

    while (glyphs_pbf.next(1)) {
        auto readGlyphMetrics = [glyphRange, &glyphs](protozero::pbf_reader& fontstack_pbf) {
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
            if (!hasID || !hasWidth || !hasHeight || !hasLeft || !hasTop || !hasAdvance || glyph.metrics.width >= 256 ||
                glyph.metrics.height >= 256 || glyph.metrics.left < -128 || glyph.metrics.left >= 128 ||
                glyph.metrics.top < -128 || glyph.metrics.top >= 128 || glyph.metrics.advance >= 256 ||
                glyph.id < glyphRange.first || glyph.id > glyphRange.second) {
                return;
            }

            // If the area of width/height is non-zero, we need to adjust the expected size
            // with the implicit border size, otherwise we expect there to be no bitmap at all.
            if (glyph.metrics.width && glyph.metrics.height) {
                const Size size{glyph.metrics.width + 2 * Glyph::borderSize,
                                glyph.metrics.height + 2 * Glyph::borderSize};

                if (size.area() != glyphData.size()) {
                    return;
                }

                glyph.bitmap = AlphaImage(size, reinterpret_cast<const uint8_t*>(glyphData.data()), glyphData.size());
            }

            glyphs.push_back(std::move(glyph));
        };

        auto fontstack_pbf = glyphs_pbf.get_message();
        while (fontstack_pbf.next()) {
            switch (fontstack_pbf.tag()) {
                case 3: {
                    readGlyphMetrics(fontstack_pbf);
                    break;
                }
                case 4: {
                    // ascender value for one fontstack shall keep the same, if different values appear, set ascender to
                    // be 0/invalid.
                    const auto value = fontstack_pbf.get_sint32();
                    if (!ascenderSet) {
                        ascender = value;
                        ascenderSet = true;
                    } else if (ascender != value) {
                        ascender = 0;
                    }
                    break;
                }
                case 5: {
                    // descender value for one fontstack shall keep the same, if different values appear, set descender
                    // to be 0/invalid.
                    const auto value = fontstack_pbf.get_sint32();
                    if (!descenderSet) {
                        descender = value;
                        descenderSet = true;
                    } else if (descender != value) {
                        descender = 0;
                    }
                    break;
                }
                default: {
                    fontstack_pbf.skip();
                    break;
                }
            }
        }
    }

    return std::make_tuple(std::move(glyphs), ascender, descender);
}

} // namespace mbgl
