#include <mbgl/annotation/sprite_store.hpp>

#include <mbgl/platform/log.hpp>

#include <mbgl/util/exception.hpp>

namespace mbgl {

SpriteStore::SpriteStore(const float pixelRatio_) : pixelRatio(pixelRatio_) {
}

void SpriteStore::setSprite(const std::string& name, std::shared_ptr<const SpriteImage> sprite) {
    std::lock_guard<std::mutex> lock(mutex);
    _setSprite(name, sprite);
}

void SpriteStore::_setSprite(const std::string& name,
                             const std::shared_ptr<const SpriteImage>& sprite) {
    if (sprite) {
        if (sprite->pixelRatio == pixelRatio) {
            sprites.emplace(name, sprite);
            dirty.emplace(name, sprite);
            return;
        } else {
            Log::Warning(Event::Sprite, "Sprite image has wrong pixel ratio");
        }
    }
    if (sprites.erase(name) > 0) {
        dirty.emplace(name, nullptr);
    }
}

void SpriteStore::removeSprite(const std::string& name) {
    std::lock_guard<std::mutex> lock(mutex);
    _setSprite(name);
}

std::shared_ptr<const SpriteImage> SpriteStore::getSprite(const std::string& name) {
    std::lock_guard<std::mutex> lock(mutex);
    const auto it = sprites.find(name);
    return it != sprites.end() ? it->second : nullptr;
}

SpriteStore::Sprites SpriteStore::getDirty() {
    Sprites result;
    std::lock_guard<std::mutex> lock(mutex);
    dirty.swap(result);
    return result;
}

} // namespace mbgl
