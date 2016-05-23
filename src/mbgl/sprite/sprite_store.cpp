#include <mbgl/sprite/sprite_store.hpp>
#include <mbgl/sprite/sprite_store_observer.hpp>
#include <mbgl/sprite/sprite_parser.hpp>
#include <mbgl/platform/log.hpp>
#include <mbgl/storage/file_source.hpp>
#include <mbgl/storage/resource.hpp>
#include <mbgl/storage/response.hpp>
#include <mbgl/util/exception.hpp>

#include <cassert>
#include <string>

namespace mbgl {

static SpriteStoreObserver nullObserver;

struct SpriteStore::Loader {
    std::shared_ptr<const std::string> image;
    std::shared_ptr<const std::string> json;
    std::unique_ptr<AsyncRequest> jsonRequest;
    std::unique_ptr<AsyncRequest> spriteRequest;
};

SpriteStore::SpriteStore(float pixelRatio_)
    : pixelRatio(pixelRatio_ > 1 ? 2 : 1), observer(&nullObserver) {
}

SpriteStore::~SpriteStore() = default;

void SpriteStore::load(const std::string& url, FileSource& fileSource) {
    if (url.empty()) {
        // Treat a non-existent sprite as a successfully loaded empty sprite.
        loaded = true;
        return;
    }

    loader = std::make_unique<Loader>();

    loader->jsonRequest = fileSource.request(Resource::spriteJSON(url, pixelRatio), [this](Response res) {
        if (res.error) {
            observer->onSpriteError(std::make_exception_ptr(std::runtime_error(res.error->message)));
        } else if (res.notModified) {
            return;
        } else if (res.noContent) {
            loader->json = std::make_shared<const std::string>();
            emitSpriteLoadedIfComplete();
        } else {
            // Only trigger a sprite loaded event we got new data.
            loader->json = res.data;
            emitSpriteLoadedIfComplete();
        }
    });

    loader->spriteRequest = fileSource.request(Resource::spriteImage(url, pixelRatio), [this](Response res) {
        if (res.error) {
            observer->onSpriteError(std::make_exception_ptr(std::runtime_error(res.error->message)));
        } else if (res.notModified) {
            return;
        } else if (res.noContent) {
            loader->image = std::make_shared<const std::string>();
            emitSpriteLoadedIfComplete();
        } else {
            loader->image = res.data;
            emitSpriteLoadedIfComplete();
        }
    });
}

void SpriteStore::emitSpriteLoadedIfComplete() {
    assert(loader);

    if (!loader->image || !loader->json) {
        return;
    }

    auto result = parseSprite(*loader->image, *loader->json);
    if (result.is<Sprites>()) {
        loaded = true;
        setSprites(result.get<Sprites>());
        observer->onSpriteLoaded();
    } else {
        observer->onSpriteError(result.get<std::exception_ptr>());
    }
}

void SpriteStore::setObserver(SpriteStoreObserver* observer_) {
    observer = observer_;
}

void SpriteStore::dumpDebugLogs() const {
    Log::Info(Event::General, "SpriteStore::loaded: %d", loaded);
}

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
            if ((it->second->image.width != sprite->image.width || it->second->image.height != sprite->image.height)) {
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
