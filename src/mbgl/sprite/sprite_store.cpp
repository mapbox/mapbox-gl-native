#include <mbgl/sprite/sprite_store.hpp>
#include <mbgl/sprite/sprite_parser.hpp>
#include <mbgl/platform/log.hpp>
#include <mbgl/storage/file_source.hpp>
#include <mbgl/storage/resource.hpp>
#include <mbgl/storage/response.hpp>
#include <mbgl/storage/request_holder.hpp>
#include <mbgl/util/exception.hpp>
#include <mbgl/util/thread_context.hpp>
#include <mbgl/util/run_loop.hpp>

#include <string>
#include <sstream>

namespace mbgl {

struct SpriteStore::Loader {
    std::shared_ptr<const std::string> image;
    std::shared_ptr<const std::string> json;
    RequestHolder jsonRequest;
    RequestHolder spriteRequest;
};

SpriteStore::SpriteStore(float pixelRatio_)
    : pixelRatio(pixelRatio_ > 1 ? 2 : 1) {
}

SpriteStore::~SpriteStore() = default;

void SpriteStore::setURL(const std::string& url) {
    if (url.empty()) {
        // Treat a non-existent sprite as a successfully loaded empty sprite.
        loaded = true;
        return;
    }

    std::string spriteURL(url + (pixelRatio > 1 ? "@2x" : "") + ".png");
    std::string jsonURL(url + (pixelRatio > 1 ? "@2x" : "") + ".json");

    loader = std::make_unique<Loader>();

    FileSource* fs = util::ThreadContext::getFileSource();
    loader->jsonRequest = fs->request({ Resource::Kind::SpriteJSON, jsonURL }, util::RunLoop::getLoop(),
                                      [this, jsonURL](const Response& res) {
        if (res.stale) {
            // Only handle fresh responses.
            return;
        }
        loader->jsonRequest = nullptr;

        if (res.error) {
            std::stringstream message;
            message << "Failed to load [" << jsonURL << "]: " << res.error->message;
            emitSpriteLoadingFailed(message.str());
            return;
        } else {
            loader->json = res.data;
        }
        emitSpriteLoadedIfComplete();
    });

    loader->spriteRequest =
        fs->request({ Resource::Kind::SpriteImage, spriteURL }, util::RunLoop::getLoop(),
                    [this, spriteURL](const Response& res) {
            if (res.stale) {
                // Only handle fresh responses.
                return;
            }
            loader->spriteRequest = nullptr;

            if (res.error) {
                std::stringstream message;
                message << "Failed to load [" << spriteURL << "]: " << res.error->message;
                emitSpriteLoadingFailed(message.str());
                return;
            } else {
                loader->image = res.data;
            }
            emitSpriteLoadedIfComplete();
        });
}

void SpriteStore::emitSpriteLoadedIfComplete() {
    assert(loader);

    if (!loader->image || !loader->json) {
        return;
    }

    auto local = std::move(loader);
    auto result = parseSprite(*local->image, *local->json);
    if (result.is<Sprites>()) {
        loaded = true;
        setSprites(result.get<Sprites>());
        if (observer) {
            observer->onSpriteLoaded();
        }
    } else {
        emitSpriteLoadingFailed(result.get<std::string>());
    }
}

void SpriteStore::emitSpriteLoadingFailed(const std::string& message) {
    if (!observer) {
        return;
    }

    auto error = std::make_exception_ptr(util::SpriteLoadingException(message));
    observer->onSpriteLoadingFailed(error);
}

void SpriteStore::setObserver(Observer* observer_) {
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
