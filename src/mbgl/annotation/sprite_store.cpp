#include <mbgl/annotation/sprite_store.hpp>

#include <mbgl/platform/log.hpp>

namespace mbgl {

void SpriteStore::setSprite(const std::string& name, std::shared_ptr<const SpriteImage> sprite) {
    std::lock_guard<std::mutex> lock(mutex);
    _setSprite(name, sprite);
}

void SpriteStore::_setSprite(const std::string& name,
                             const std::shared_ptr<const SpriteImage>& sprite) {
    if (sprite) {
        auto it = sprites.find(name);
        if (it != sprites.end() &&
            (it->second->width != sprite->width || it->second->height != sprite->height)) {
            Log::Warning(Event::Sprite, "Can't change sprite dimensions for '%s'", name.c_str());
            return;
        }

        sprites.emplace_hint(it, name, sprite);
        dirty.emplace(name, sprite);
    } else if (sprites.erase(name) > 0) {
        dirty.emplace(name, nullptr);
    }
}

void SpriteStore::setSprites(const Sprites& newSprites) {
    std::lock_guard<std::mutex> lock(mutex);
    for (const auto& pair : newSprites) {
        _setSprite(pair.first, pair.second);
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
