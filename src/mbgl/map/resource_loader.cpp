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

    if (sprite_) {
        sprite_->setObserver(nullptr);
    }

    if (glyphStore_) {
        glyphStore_->setObserver(nullptr);
    }
}

void ResourceLoader::setObserver(Observer* observer) {
    assert(Environment::currentlyOn(ThreadType::Map));
    assert(!observer_);

    observer_ = observer;
}

void ResourceLoader::setStyle(Style* style) {
    assert(style);

    style_ = style;

    for (const auto& source : style->sources) {
        source->setObserver(this);
        source->load();
    }
}

void ResourceLoader::setGlyphStore(GlyphStore* glyphStore) {
    assert(glyphStore);

    if (glyphStore_) {
        glyphStore_->setObserver(nullptr);
    }

    glyphStore_ = glyphStore;
    glyphStore_->setObserver(this);
}

void ResourceLoader::update(MapData& data,
                            const TransformState& transform,
                            GlyphAtlas& glyphAtlas,
                            SpriteAtlas& spriteAtlas,
                            TexturePool& texturePool) {
    if (!style_) {
        return;
    }

    const float pixelRatio = transform.getPixelRatio();
    if (!sprite_ || !sprite_->hasPixelRatio(pixelRatio)) {
        sprite_ = std::make_unique<Sprite>(style_->getSpriteURL(), pixelRatio);
        sprite_->setObserver(this);

        spriteAtlas.resize(pixelRatio);
        spriteAtlas.setSprite(sprite_);
    }

    bool allTilesUpdated = true;
    for (const auto& source : style_->sources) {
        if (!source->update(data, transform, *style_, glyphAtlas, *glyphStore_,
                       spriteAtlas, sprite_, texturePool, shouldReparsePartialTiles_)) {
            allTilesUpdated = false;
        }
    }

    // We can only stop updating "partial" tiles when all of them
    // were notified of the arrival of the new resources.
    if (allTilesUpdated) {
        shouldReparsePartialTiles_ = false;
    }
}

void ResourceLoader::onGlyphRangeLoaded() {
    shouldReparsePartialTiles_ = true;

    emitTileDataChanged();
}

void ResourceLoader::onGlyphRangeLoadingFailed(std::exception_ptr error) {
    emitResourceLoadingFailed(error);
}

void ResourceLoader::onSourceLoaded() {
    emitTileDataChanged();
}

void ResourceLoader::onSourceLoadingFailed(std::exception_ptr error) {
    emitResourceLoadingFailed(error);
}

void ResourceLoader::onTileLoaded(bool isNewTile) {
    if (isNewTile) {
        shouldReparsePartialTiles_ = true;
    }

    emitTileDataChanged();
}

void ResourceLoader::onTileLoadingFailed(std::exception_ptr error) {
    emitResourceLoadingFailed(error);
}

void ResourceLoader::onSpriteLoaded() {
    shouldReparsePartialTiles_ = true;

    emitTileDataChanged();
}

void ResourceLoader::onSpriteLoadingFailed(std::exception_ptr error) {
    emitResourceLoadingFailed(error);
}

void ResourceLoader::emitTileDataChanged() {
    assert(Environment::currentlyOn(ThreadType::Map));

    if (observer_) {
        observer_->onTileDataChanged();
    }
}

void ResourceLoader::emitResourceLoadingFailed(std::exception_ptr error) {
    assert(Environment::currentlyOn(ThreadType::Map));

    try {
        if (error) {
            std::rethrow_exception(error);
        }
    } catch(const std::exception& e) {
        Log::Error(Event::ResourceLoader, e.what());
    }

    if (observer_) {
        observer_->onResourceLoadingFailed(error);
    }
}

}
