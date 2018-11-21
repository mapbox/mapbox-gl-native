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

const mapbox::Bin& _packImage(mapbox::ShelfPack& pack, const style::Image::Impl& image, ImageAtlas& resultImage, ImageType imageType) {
    const mapbox::Bin& bin = *pack.packOne(-1,
        image.image.size.width + 2 * padding,
        image.image.size.height + 2 * padding);

    resultImage.image.resize({
        static_cast<uint32_t>(pack.width()),
        static_cast<uint32_t>(pack.height())
    });

    PremultipliedImage::copy(image.image,
                             resultImage.image,
                             { 0, 0 },
                             {
                                bin.x + padding,
                                bin.y + padding
                             },
                             image.image.size);
    uint32_t x = bin.x + padding,
            y = bin.y + padding,
            w = image.image.size.width,
            h = image.image.size.height;

    if (imageType == ImageType::Pattern) {
            // Add 1 pixel wrapped padding on each side of the image.
        PremultipliedImage::copy(image.image, resultImage.image, { 0, h - 1 }, { x, y - 1 }, { w, 1 }); // T
        PremultipliedImage::copy(image.image, resultImage.image, { 0,     0 }, { x, y + h }, { w, 1 }); // B
        PremultipliedImage::copy(image.image, resultImage.image, { w - 1, 0 }, { x - 1, y }, { 1, h }); // L
        PremultipliedImage::copy(image.image, resultImage.image, { 0,     0 }, { x + w, y }, { 1, h }); // R
    }
    return bin;
}

ImageAtlas makeImageAtlas(const ImageMap& icons, const ImageMap& patterns) {
    ImageAtlas result;

    mapbox::ShelfPack::ShelfPackOptions options;
    options.autoResize = true;
    mapbox::ShelfPack pack(0, 0, options);

    for (const auto& entry : icons) {
        const style::Image::Impl& image = *entry.second;
        const mapbox::Bin& bin = _packImage(pack, image, result, ImageType::Icon);
        result.iconPositions.emplace(image.id, ImagePosition { bin, image });
    }

    for (const auto& entry : patterns) {
        const style::Image::Impl& image = *entry.second;
        const mapbox::Bin& bin = _packImage(pack, image, result, ImageType::Pattern);
        result.patternPositions.emplace(image.id, ImagePosition { bin, image });
    }

    pack.shrink();
    result.image.resize({
        static_cast<uint32_t>(pack.width()),
        static_cast<uint32_t>(pack.height())
    });

    return result;
}

} // namespace mbgl
