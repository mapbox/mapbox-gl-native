#include <mbgl/renderer/tile_render_data.hpp>

namespace mbgl {

TileRenderData::TileRenderData() = default;

TileRenderData::TileRenderData(std::shared_ptr<TileAtlasTextures> atlasTextures_)
    : atlasTextures(std::move(atlasTextures_)) {
}

TileRenderData::~TileRenderData() = default;

const gfx::Texture& TileRenderData::getGlyphAtlasTexture() const {
    assert(atlasTextures);
    assert(atlasTextures->glyph);
    return *atlasTextures->glyph;
}

const gfx::Texture& TileRenderData::getIconAtlasTexture() const {
    assert(atlasTextures);
    assert(atlasTextures->icon);
    return *atlasTextures->icon;
}

optional<ImagePosition> TileRenderData::getPattern(const std::string&) const {
    assert(false);
    return nullopt;
}

const LayerRenderData* TileRenderData::getLayerRenderData(const style::Layer::Impl&) const {
    assert(false);
    return nullptr;
}

Bucket* TileRenderData::getBucket(const style::Layer::Impl&) const {
    assert(false);
    return nullptr;
}

} // namespace mbgl
