#include <mbgl/annotation/sprite_store.hpp>

#include <mbgl/util/exception.hpp>

namespace mbgl {

SpriteStore::SpriteStore(const float ratio_) : ratio(ratio_) {
}

void SpriteStore::setSprite(const std::string& name, std::shared_ptr<const SpriteImage> sprite) {
    if (sprite && sprite->ratio != ratio) {
        throw util::SpriteImageException("Sprite image has wrong pixel ratio");
    }
    std::lock_guard<std::mutex> lock(mutex);
    if (sprite) {
        sprites.emplace(name, sprite);
    } else {
        sprites.erase(name);
    }
    dirty.emplace(name, sprite);
}

void SpriteStore::removeSprite(const std::string& name) {
    setSprite(name);
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
