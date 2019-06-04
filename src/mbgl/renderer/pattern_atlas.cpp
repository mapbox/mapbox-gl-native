#include <mbgl/renderer/pattern_atlas.hpp>
#include <mbgl/gfx/upload_pass.hpp>
#include <mbgl/gfx/context.hpp>

namespace mbgl {

namespace {

// When copied into the atlas texture, image data is padded by one pixel on each side.
// Pattern images are padded with a copy of the image data wrapped from the opposite side.
// This ensures the correct behavior of GL_LINEAR texture sampling mode.
const uint16_t padding = 1;

mapbox::ShelfPack::ShelfPackOptions shelfPackOptions() {
    mapbox::ShelfPack::ShelfPackOptions options;
    options.autoResize = true;
    return options;
}

} // namespace

PatternAtlas::PatternAtlas()
    : shelfPack(64, 64, shelfPackOptions()) {
}

PatternAtlas::~PatternAtlas() = default;

optional<ImagePosition> PatternAtlas::getPattern(const std::string& id) const {
    auto it = patterns.find(id);
    if (it != patterns.end()) {
        return it->second.position;
    }
    return nullopt;
}

optional<ImagePosition> PatternAtlas::addPattern(const style::Image::Impl& image) {
    if (patterns.find(image.id) != patterns.end()) {
        return nullopt;
    }
    const uint16_t width = image.image.size.width + padding * 2;
    const uint16_t height = image.image.size.height + padding * 2;

    mapbox::Bin* bin = shelfPack.packOne(-1, width, height);
    if (!bin) {
        return nullopt;
    }

    atlasImage.resize(getPixelSize());

    const PremultipliedImage& src = image.image;

    const uint32_t x = bin->x + padding;
    const uint32_t y = bin->y + padding;
    const uint32_t w = src.size.width;
    const uint32_t h = src.size.height;

    PremultipliedImage::copy(src, atlasImage, { 0, 0 }, { x, y }, { w, h });

    // Add 1 pixel wrapped padding on each side of the image.
    PremultipliedImage::copy(src, atlasImage, { 0, h - 1 }, { x, y - 1 }, { w, 1 }); // T
    PremultipliedImage::copy(src, atlasImage, { 0,     0 }, { x, y + h }, { w, 1 }); // B
    PremultipliedImage::copy(src, atlasImage, { w - 1, 0 }, { x - 1, y }, { 1, h }); // L
    PremultipliedImage::copy(src, atlasImage, { 0,     0 }, { x + w, y }, { 1, h }); // R

    dirty = true;

    return patterns.emplace(image.id, Pattern { bin, { *bin, image } }).first->second.position;
}

void PatternAtlas::removePattern(const std::string& id) {
    auto it = patterns.find(id);
    if (it != patterns.end()) {
        // Clear pattern from the atlas image.
        const uint32_t x = it->second.bin->x;
        const uint32_t y = it->second.bin->y;
        const uint32_t w = it->second.bin->w;
        const uint32_t h = it->second.bin->h;
        PremultipliedImage::clear(atlasImage, { x, y }, { w, h });

        shelfPack.unref(*it->second.bin);
        patterns.erase(it);
    }
}

Size PatternAtlas::getPixelSize() const {
    return {
        static_cast<uint32_t>(shelfPack.width()),
        static_cast<uint32_t>(shelfPack.height())
    };
}

void PatternAtlas::upload(gfx::UploadPass& uploadPass) {
    if (!atlasTexture) {
        atlasTexture = uploadPass.createTexture(atlasImage);
    } else if (dirty) {
        uploadPass.updateTexture(*atlasTexture, atlasImage);
    }

    dirty = false;
}

gfx::TextureBinding PatternAtlas::textureBinding() const {
    assert(atlasTexture);
    assert(!dirty);
    return { atlasTexture->getResource(), gfx::TextureFilterType::Linear };
}

} // namespace mbgl
