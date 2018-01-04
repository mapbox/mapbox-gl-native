#include <mbgl/sprite/sprite_loader.hpp>
#include <mbgl/sprite/sprite_loader_worker.hpp>
#include <mbgl/sprite/sprite_loader_observer.hpp>
#include <mbgl/sprite/sprite_parser.hpp>
#include <mbgl/util/logging.hpp>
#include <mbgl/util/platform.hpp>
#include <mbgl/util/std.hpp>
#include <mbgl/util/constants.hpp>
#include <mbgl/util/exception.hpp>
#include <mbgl/storage/file_source.hpp>
#include <mbgl/storage/resource.hpp>
#include <mbgl/storage/response.hpp>
#include <mbgl/actor/actor.hpp>
#include <mbgl/actor/scheduler.hpp>

#include <cassert>

namespace mbgl {

static SpriteLoaderObserver nullObserver;

struct SpriteLoader::Loader {
    Loader(Scheduler& scheduler, SpriteLoader& imageManager)
        : mailbox(std::make_shared<Mailbox>(*Scheduler::GetCurrent())),
          worker(scheduler, ActorRef<SpriteLoader>(imageManager, mailbox)) {
    }

    std::shared_ptr<const std::string> image;
    std::shared_ptr<const std::string> json;
    std::unique_ptr<AsyncRequest> jsonRequest;
    std::unique_ptr<AsyncRequest> spriteRequest;
    std::shared_ptr<Mailbox> mailbox;
    Actor<SpriteLoaderWorker> worker;
};

SpriteLoader::SpriteLoader(float pixelRatio_)
        : pixelRatio(pixelRatio_)
        , observer(&nullObserver) {
}

SpriteLoader::~SpriteLoader() = default;

void SpriteLoader::load(const std::string& url, Scheduler& scheduler, FileSource& fileSource) {
    if (url.empty()) {
        // Treat a non-existent sprite as a successfully loaded empty sprite.
        observer->onSpriteLoaded({});
        return;
    }

    loader = std::make_unique<Loader>(scheduler, *this);

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

void SpriteLoader::emitSpriteLoadedIfComplete() {
    assert(loader);

    if (!loader->image || !loader->json) {
        return;
    }

    loader->worker.invoke(&SpriteLoaderWorker::parse, loader->image, loader->json);
}

void SpriteLoader::onParsed(std::vector<std::unique_ptr<style::Image>>&& result) {
    observer->onSpriteLoaded(std::move(result));
}

void SpriteLoader::onError(std::exception_ptr err) {
    observer->onSpriteError(err);
}

void SpriteLoader::setObserver(SpriteLoaderObserver* observer_) {
    observer = observer_;
}

} // namespace mbgl
