#include <mbgl/renderer/image_atlas.hpp>

#include <mapbox/shelf-pack.hpp>

namespace mbgl {

static constexpr uint32_t padding = 1;

ImagePosition::ImagePosition(const mapbox::Bin& bin, const style::Image::Impl& image)
    : pixelRatio(image.pixelRatio),
      textureRect(
        bin.x + padding,
        bin.y + padding,
        bin.w - padding * 2,
        bin.h - padding * 2
      ) {
}

ImageAtlas makeImageAtlas(const ImageMap& images) {
    ImageAtlas result;

    mapbox::ShelfPack::ShelfPackOptions options;
    options.autoResize = true;
    mapbox::ShelfPack pack(0, 0, options);

    for (const auto& entry : images) {
        const style::Image::Impl& image = *entry.second;

        const mapbox::Bin& bin = *pack.packOne(-1,
            image.image.size.width + 2 * padding,
            image.image.size.height + 2 * padding);

        result.image.resize({
            static_cast<uint32_t>(pack.width()),
            static_cast<uint32_t>(pack.height())
        });

        PremultipliedImage::copy(image.image,
                                 result.image,
                                 { 0, 0 },
                                 {
                                    bin.x + padding,
                                    bin.y + padding
                                 },
                                 image.image.size);

        result.positions.emplace(image.id,
                                 ImagePosition { bin, image });
    }

    pack.shrink();
    result.image.resize({
        static_cast<uint32_t>(pack.width()),
        static_cast<uint32_t>(pack.height())
    });

    return result;
}

} // namespace mbgl
