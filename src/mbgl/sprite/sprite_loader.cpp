#include <mbgl/sprite/sprite_loader.hpp>
#include <mbgl/sprite/sprite_loader_observer.hpp>
#include <mbgl/sprite/sprite_parser.hpp>
#include <mbgl/util/platform.hpp>
#include <mbgl/util/std.hpp>
#include <mbgl/util/constants.hpp>
#include <mbgl/util/exception.hpp>
#include <mbgl/storage/file_source.hpp>
#include <mbgl/storage/resource.hpp>
#include <mbgl/storage/response.hpp>
#include <mbgl/actor/actor.hpp>

#include <cassert>

namespace mbgl {

SpriteLoader::SpriteLoader(ActorRef<SpriteLoader> self_, ActorRef<SpriteLoaderObserver> observer_, FileSource& fileSource_, float pixelRatio_)
        : self(self_)
        , observer(observer_)
        , fileSource(fileSource_)
        , pixelRatio(pixelRatio_) {
}

SpriteLoader::~SpriteLoader() = default;

void SpriteLoader::load(const std::string& url) {
    if (url.empty()) {
        // Treat a non-existent sprite as a successfully loaded empty sprite.
        observer.invoke(&SpriteLoaderObserver::onSpriteLoaded, SpriteLoaderObserver::Images());
        return;
    }

    jsonRequest = fileSource.request(Resource::spriteJSON(url, pixelRatio), [this](Response res) {
        if (res.error) {
            observer.invoke(&SpriteLoaderObserver::onSpriteError, std::make_exception_ptr(std::runtime_error(res.error->message)));
        } else if (res.notModified) {
            return;
        } else if (res.noContent) {
            json = std::make_shared<const std::string>();
            parseIfComplete();
        } else {
            // Only trigger a sprite loaded event we got new data.
            json = res.data;
            parseIfComplete();
        }
    });

    spriteRequest = fileSource.request(Resource::spriteImage(url, pixelRatio), [this](Response res) {
        if (res.error) {
            observer.invoke(&SpriteLoaderObserver::onSpriteError, std::make_exception_ptr(std::runtime_error(res.error->message)));
        } else if (res.notModified) {
            return;
        } else if (res.noContent) {
            image = std::make_shared<const std::string>();
            parseIfComplete();
        } else {
            image = res.data;
            parseIfComplete();
        }
    });
}

void SpriteLoader::parse() {
    try {
        if (!image) {
            // This shouldn't happen, since we always invoke it with a non-empty pointer.
            throw std::runtime_error("missing sprite image");
        }
        if (!json) {
            // This shouldn't happen, since we always invoke it with a non-empty pointer.
            throw std::runtime_error("missing sprite metadata");
        }

        self.invoke(&SpriteLoader::onParsed, parseSprite(*image, *json));
    } catch (...) {
        self.invoke(&SpriteLoader::onError, std::current_exception());
    }
}

void SpriteLoader::parseIfComplete() {
    if (!image || !json) {
        return;
    }

    self.invoke(&SpriteLoader::parse);
}

void SpriteLoader::onParsed(std::vector<std::unique_ptr<style::Image>>&& result) {
    observer.invoke(&SpriteLoaderObserver::onSpriteLoaded, std::move(result));
}

void SpriteLoader::onError(std::exception_ptr err) {
    observer.invoke(&SpriteLoaderObserver::onSpriteError, err);
}

} // namespace mbgl
