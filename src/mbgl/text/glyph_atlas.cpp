#include <mbgl/text/glyph_atlas.hpp>

#include <mapbox/shelf-pack.hpp>

namespace mbgl {

static constexpr uint32_t padding = 1;

GlyphAtlas makeGlyphAtlas(const GlyphMap& glyphs) {
    GlyphAtlas result;

    mapbox::ShelfPack::ShelfPackOptions options;
    options.autoResize = true;
    mapbox::ShelfPack pack(0, 0, options);

    for (const auto& glyphMapEntry : glyphs) {
        const FontStack& fontStack = glyphMapEntry.first;
        GlyphPositionMap& positions = result.positions[fontStack];

        for (const auto& entry : glyphMapEntry.second) {
            if (entry.second && (*entry.second)->bitmap.valid()) {
                const Glyph& glyph = **entry.second;

                const mapbox::Bin& bin = *pack.packOne(-1,
                    glyph.bitmap.size.width + 2 * padding,
                    glyph.bitmap.size.height + 2 * padding);

                result.image.resize({
                    static_cast<uint32_t>(pack.width()),
                    static_cast<uint32_t>(pack.height())
                });

                AlphaImage::copy(glyph.bitmap,
                                 result.image,
                                 { 0, 0 },
                                 {
                                    bin.x + padding,
                                    bin.y + padding
                                 },
                                 glyph.bitmap.size);

                positions.emplace(glyph.id,
                                  GlyphPosition {
                                     Rect<uint16_t> {
                                         static_cast<uint16_t>(bin.x),
                                         static_cast<uint16_t>(bin.y),
                                         static_cast<uint16_t>(bin.w),
                                         static_cast<uint16_t>(bin.h)
                                     },
                                     glyph.metrics
                                  });
            }
        }
    }

    pack.shrink();
    result.image.resize({
        static_cast<uint32_t>(pack.width()),
        static_cast<uint32_t>(pack.height())
    });

    return result;
}

} // namespace mbgl
