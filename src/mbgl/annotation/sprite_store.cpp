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
        if (it != sprites.end()) {
            // There is already a sprite with that name in our store.
            if ((it->second->width != sprite->width || it->second->height != sprite->height)) {
                Log::Warning(Event::Sprite, "Can't change sprite dimensions for '%s'", name.c_str());
                return;
            }
            it->second = sprite;
        } else {
            sprites.emplace(name, sprite);
        }

        // Always add/replace the value in the dirty list.
        auto dirty_it = dirty.find(name);
        if (dirty_it != dirty.end()) {
            dirty_it->second = sprite;
        } else {
            dirty.emplace(name, sprite);
        }
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
    if (it != sprites.end()) {
        return it->second;
    } else {
        if (!sprites.empty()) {
            Log::Info(Event::Sprite, "Can't find sprite named '%s'", name.c_str());
        }
        return nullptr;
    }
}

SpriteStore::Sprites SpriteStore::getDirty() {
    Sprites result;
    std::lock_guard<std::mutex> lock(mutex);
    dirty.swap(result);
    return result;
}

} // namespace mbgl
