#include <mbgl/map/sprite.hpp>

#include <mbgl/platform/log.hpp>
#include <mbgl/platform/platform.hpp>
#include <mbgl/storage/file_source.hpp>
#include <mbgl/storage/resource.hpp>
#include <mbgl/storage/response.hpp>
#include <mbgl/storage/request_holder.hpp>
#include <mbgl/util/exception.hpp>
#include <mbgl/util/raster.hpp>
#include <mbgl/util/thread.hpp>
#include <mbgl/util/uv_detail.hpp>
#include <mbgl/util/mapbox.hpp>

#include <rapidjson/document.h>

#include <string>
#include <sstream>

namespace mbgl {

struct Sprite::Loader {
    std::shared_ptr<const std::string> image;
    std::shared_ptr<const std::string> json;
    RequestHolder jsonRequest;
    RequestHolder spriteRequest;
};

Sprite::Sprite(const std::string& baseUrl, float pixelRatio_)
    : pixelRatio(pixelRatio_ > 1 ? 2 : 1) {
    if (baseUrl.empty()) {
        // Treat a non-existent sprite as a successfully loaded empty sprite.
        loaded = true;
        return;
    }

    std::string spriteURL(baseUrl + (pixelRatio_ > 1 ? "@2x" : "") + ".png");
    std::string jsonURL(baseUrl + (pixelRatio_ > 1 ? "@2x" : "") + ".json");

    loader = std::make_unique<Loader>();

    FileSource* fs = util::ThreadContext::getFileSource();
    loader->jsonRequest = fs->request({ Resource::Kind::SpriteJSON, jsonURL }, util::RunLoop::getLoop(),
                                      [this, jsonURL](const Response& res) {
        if (res.stale) {
            // Only handle fresh responses.
            return;
        }
        loader->jsonRequest = nullptr;
        if (res.status == Response::Successful) {
            loader->json = res.data;
        } else {
            std::stringstream message;
            message << "Failed to load [" << jsonURL << "]: " << res.message;
            emitSpriteLoadingFailed(message.str());
            return;
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
            if (res.status == Response::Successful) {
                loader->image = res.data;
            } else {
                std::stringstream message;
                message << "Failed to load [" << spriteURL << "]: " << res.message;
                emitSpriteLoadingFailed(message.str());
                return;
            }
            emitSpriteLoadedIfComplete();
        });
}

Sprite::~Sprite() {
}

void Sprite::emitSpriteLoadedIfComplete() {
    assert(loader);

    if (!loader->image || !loader->json || !observer) {
        return;
    }

    auto local = std::move(loader);
    auto result = parseSprite(*local->image, *local->json);
    if (result.is<Sprites>()) {
        loaded = true;
        observer->onSpriteLoaded(result.get<Sprites>());
    } else {
        emitSpriteLoadingFailed(result.get<std::string>());
    }
}

void Sprite::emitSpriteLoadingFailed(const std::string& message) {
    if (!observer) {
        return;
    }

    auto error = std::make_exception_ptr(util::SpriteLoadingException(message));
    observer->onSpriteLoadingFailed(error);
}

void Sprite::setObserver(Observer* observer_) {
    observer = observer_;
}

} // namespace mbgl
