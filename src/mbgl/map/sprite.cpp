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

#include <rapidjson/document.h>

#include <string>
#include <sstream>

using namespace mbgl;

Sprite::Sprite(const std::string& baseUrl, float pixelRatio_)
    : pixelRatio(pixelRatio_ > 1 ? 2 : 1) {
    if (baseUrl.empty()) {
        // Treat a non-existent sprite as a successfully loaded empty sprite.
        loadedImage = true;
        loadedJSON = true;
        return;
    }

    std::string spriteURL(baseUrl + (pixelRatio_ > 1 ? "@2x" : "") + ".png");
    std::string jsonURL(baseUrl + (pixelRatio_ > 1 ? "@2x" : "") + ".json");

    FileSource* fs = util::ThreadContext::getFileSource();
    jsonRequest = fs->request({ Resource::Kind::JSON, jsonURL }, util::RunLoop::getLoop(), [this, jsonURL](const Response &res) {
        jsonRequest = nullptr;
        if (res.status == Response::Successful) {
            json = res.data;
            loadedJSON = true;
        } else {
            std::stringstream message;
            message <<  "Failed to load [" << jsonURL << "]: " << res.message;
            emitSpriteLoadingFailed(message.str());
            return;
        }
        emitSpriteLoadedIfComplete();
    });

    spriteRequest = fs->request({ Resource::Kind::Image, spriteURL }, util::RunLoop::getLoop(), [this, spriteURL](const Response &res) {
        spriteRequest = nullptr;
        if (res.status == Response::Successful) {
            image = res.data;
            loadedImage = true;
        } else {
            std::stringstream message;
            message <<  "Failed to load [" << spriteURL << "]: " << res.message;
            emitSpriteLoadingFailed(message.str());
            return;
        }
        emitSpriteLoadedIfComplete();
    });
}

Sprite::~Sprite() {
    if (jsonRequest) {
        util::ThreadContext::getFileSource()->cancel(jsonRequest);
    }

    if (spriteRequest) {
        util::ThreadContext::getFileSource()->cancel(spriteRequest);
    }
}

void Sprite::emitSpriteLoadedIfComplete() {
    if (isLoaded() && observer) {
        observer->onSpriteLoaded();
    }
}

void Sprite::emitSpriteLoadingFailed(const std::string& message) {
    if (!observer) {
        return;
    }

    auto error = std::make_exception_ptr(util::SpriteLoadingException(message));
    observer->onSpriteLoadingFailed(error);
}

bool Sprite::isLoaded() const {
    return loadedImage && loadedJSON;
}

void Sprite::setObserver(Observer* observer_) {
    observer = observer_;
}
