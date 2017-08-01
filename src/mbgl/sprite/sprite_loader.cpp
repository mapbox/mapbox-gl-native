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
#include <mbgl/storage/resource_error.hpp>
#include <mbgl/util/run_loop.hpp>
#include <mbgl/actor/actor.hpp>

#include <cassert>

namespace mbgl {

static SpriteLoaderObserver nullObserver;

struct SpriteLoader::Loader {
    Loader(Scheduler& scheduler, SpriteLoader& imageManager)
        : mailbox(std::make_shared<Mailbox>(*util::RunLoop::Get())),
          worker(scheduler, ActorRef<SpriteLoader>(imageManager, mailbox)) {
    }

    std::shared_ptr<const std::string> image;
    std::shared_ptr<const std::string> json;
    std::unique_ptr<AsyncRequest> jsonRequest;
    std::unique_ptr<AsyncRequest> spriteRequest;
    std::shared_ptr<Mailbox> mailbox;
    Actor<SpriteLoaderWorker> worker;
    uint64_t correlationID = 0;

    bool parsed = false;
    bool pending = false;
};

SpriteLoader::SpriteLoader(float pixelRatio_)
        : pixelRatio(pixelRatio_)
        , observer(&nullObserver) {
}

SpriteLoader::~SpriteLoader() = default;

void SpriteLoader::load(const std::string& url, Scheduler& scheduler, FileSource& fileSource) {
    if (url.empty()) {
        // Treat a non-existent sprite as a successfully loaded empty sprite.
        loader.reset();
        observer->onSpriteLoaded({});
        return;
    }

    loader = std::make_unique<Loader>(scheduler, *this);

    const auto jsonResource = Resource::spriteJSON(url, pixelRatio);
    loader->jsonRequest = fileSource.request(jsonResource, [this, url = jsonResource.url](Response res) {
        if (res.error) {
            const auto severity = loader->json ? EventSeverity::Warning : EventSeverity::Error;
            util::ResourceError err(res.error->message, ResourceKind::SpriteJSON, res.error->status,
                                    url);
            observer->onSpriteError(std::make_exception_ptr(err), severity);
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

    const auto spriteResource = Resource::spriteImage(url, pixelRatio);
    loader->spriteRequest = fileSource.request(spriteResource, [this, url = spriteResource.url](Response res) {
        if (res.error) {
            const auto severity = loader->json ? EventSeverity::Warning : EventSeverity::Error;
            util::ResourceError err(res.error->message, ResourceKind::SpriteImage,
                                    res.error->status, url);
            observer->onSpriteError(std::make_exception_ptr(err), severity);
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

bool SpriteLoader::isLoaded() const {
    return !loader || (loader->parsed && !loader->pending);
}

void SpriteLoader::emitSpriteLoadedIfComplete() {
    assert(loader);

    if (!loader->image || !loader->json) {
        return;
    }

    loader->pending = true;

    ++loader->correlationID;
    loader->worker.invoke(&SpriteLoaderWorker::parse, loader->image, loader->json,
                          loader->correlationID);
}

void SpriteLoader::onParsed(std::vector<std::unique_ptr<style::Image>>&& result, const uint64_t resultCorrelationID) {
    loader->parsed = true;
    if (resultCorrelationID == loader->correlationID) {
        loader->pending = false;
    }
    observer->onSpriteLoaded(std::move(result));
}

void SpriteLoader::onError(std::exception_ptr err, uint64_t const resultCorrelationID) {
    loader->parsed = true;
    if (resultCorrelationID == loader->correlationID) {
        loader->pending = false;
    }
    observer->onSpriteError(err);
}

void SpriteLoader::setObserver(SpriteLoaderObserver* observer_) {
    observer = observer_;
}

} // namespace mbgl

