#include <mbgl/map/resource_loader.hpp>

#include <mbgl/geometry/sprite_atlas.hpp>
#include <mbgl/map/environment.hpp>
#include <mbgl/map/source.hpp>
#include <mbgl/map/sprite.hpp>
#include <mbgl/map/transform.hpp>
#include <mbgl/style/style.hpp>

#include <cassert>

namespace mbgl {

ResourceLoader::ResourceLoader() {
    assert(Environment::currentlyOn(ThreadType::Map));
}

ResourceLoader::~ResourceLoader() {
    assert(Environment::currentlyOn(ThreadType::Map));

    for (const auto& source : style_->sources) {
        source->setObserver(nullptr);
    }

    sprite_->setObserver(nullptr);
}

void ResourceLoader::setObserver(Observer* observer) {
    assert(Environment::currentlyOn(ThreadType::Map));
    assert(!observer_);

    observer_ = observer;
}

void ResourceLoader::setStyle(Style* style) {
    assert(style);

    style_ = style;

    Environment& env = Environment::Get();
    for (const auto& source : style->sources) {
        source->setObserver(this);
        source->load(accessToken_, env);
    }
}

void ResourceLoader::setAccessToken(const std::string& accessToken) {
    accessToken_ = accessToken;
}

void ResourceLoader::update(MapData& data,
                            const TransformState& transform,
                            GlyphAtlas& glyphAtlas,
                            GlyphStore& glyphStore,
                            SpriteAtlas& spriteAtlas,
                            TexturePool& texturePool) {
    if (!style_) {
        return;
    }

    const float pixelRatio = transform.getPixelRatio();
    if (!sprite_ || !sprite_->hasPixelRatio(pixelRatio)) {
        sprite_ = util::make_unique<Sprite>(style_->getSpriteURL(), pixelRatio);
        sprite_->setObserver(this);

        spriteAtlas.resize(pixelRatio);
        spriteAtlas.setSprite(sprite_);
    }

    for (const auto& source : style_->sources) {
        source->update(
            data, transform, *style_, glyphAtlas, glyphStore, spriteAtlas, sprite_, texturePool);
    }
}

void ResourceLoader::onGlyphRangeLoaded() {
    emitTileDataChanged();
}

void ResourceLoader::onSourceLoaded() {
    emitTileDataChanged();
}

void ResourceLoader::onTileLoaded() {
    emitTileDataChanged();
}

void ResourceLoader::onSpriteLoaded() {
    emitTileDataChanged();
}

void ResourceLoader::emitTileDataChanged() {
    assert(Environment::currentlyOn(ThreadType::Map));

    if (observer_) {
        observer_->onTileDataChanged();
    }
}

}
