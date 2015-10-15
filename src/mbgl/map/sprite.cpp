#include <mbgl/map/sprite.hpp>

#include <mbgl/platform/log.hpp>
#include <mbgl/platform/platform.hpp>
#include <mbgl/storage/file_source.hpp>
#include <mbgl/storage/resource.hpp>
#include <mbgl/storage/response.hpp>
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
    bool loadedJSON = false;
    bool loadedImage = false;
    std::unique_ptr<Data> data = std::make_unique<Data>();

    Request* jsonRequest = nullptr;
    Request* spriteRequest = nullptr;

    ~Loader() {
        if (jsonRequest) {
            util::ThreadContext::getFileSource()->cancel(jsonRequest);
            jsonRequest = nullptr;
        }
        if (spriteRequest) {
            util::ThreadContext::getFileSource()->cancel(spriteRequest);
            spriteRequest = nullptr;
        }
    }
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
        util::ThreadContext::getFileSource()->cancel(loader->jsonRequest);
        loader->jsonRequest = nullptr;
        if (res.status == Response::Successful) {
            loader->data->json = res.data;
            loader->loadedJSON = true;
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
            util::ThreadContext::getFileSource()->cancel(loader->spriteRequest);
            loader->spriteRequest = nullptr;
            if (res.status == Response::Successful) {
                loader->data->image = res.data;
                loader->loadedImage = true;
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

    if (!loader->loadedImage || !loader->loadedJSON || !observer) {
        return;
    }

    std::unique_ptr<Data> data(std::move(loader->data));
    loader.reset();

    auto result = parseSprite(data->image, data->json);
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
