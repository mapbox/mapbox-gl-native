#include <mbgl/text/glyph_atlas.hpp>

#include <mapbox/shelf-pack.hpp>

namespace mbgl {

static constexpr uint32_t padding = 1;

GlyphAtlas makeGlyphAtlas(const Glyphs& glyphs) {
    GlyphAtlas result;

    mapbox::ShelfPack::ShelfPackOptions options;
    options.autoResize = true;
    mapbox::ShelfPack pack(0, 0, options);

    std::vector<mapbox::Bin> bins;
    bins.reserve(glyphs.size());

    for (const auto& entry : glyphs) {
        if (entry.second && (*entry.second)->bitmap.valid()) {
            const Glyph& glyph = **entry.second;
            bins.emplace_back(glyph.id,
                              glyph.bitmap.size.width + 2 * padding,
                              glyph.bitmap.size.height + 2 * padding);
        }
    }

    mapbox::ShelfPack::PackOptions packOptions;
    packOptions.inPlace = true;
    pack.pack(bins, packOptions);

    result.image = AlphaImage({
        static_cast<uint32_t>(pack.width()),
        static_cast<uint32_t>(pack.height())
    });

    for (const auto& bin : bins) {
        const Glyph& glyph = **glyphs.at(bin.id);

        AlphaImage::copy(glyph.bitmap,
                         result.image,
                         { 0, 0 },
                         {
                            bin.x + padding,
                            bin.y + padding
                         },
                         glyph.bitmap.size);

        result.positions.emplace(glyph.id,
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

    return result;
}

} // namespace mbgl
