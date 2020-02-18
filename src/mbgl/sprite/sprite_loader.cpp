#include <mbgl/actor/actor.hpp>
#include <mbgl/actor/scheduler.hpp>
#include <mbgl/sprite/sprite_loader.hpp>
#include <mbgl/sprite/sprite_loader_observer.hpp>
#include <mbgl/sprite/sprite_parser.hpp>
#include <mbgl/storage/file_source.hpp>
#include <mbgl/storage/resource.hpp>
#include <mbgl/storage/response.hpp>
#include <mbgl/util/async_request.hpp>
#include <mbgl/util/constants.hpp>
#include <mbgl/util/exception.hpp>
#include <mbgl/util/logging.hpp>
#include <mbgl/util/platform.hpp>
#include <mbgl/util/std.hpp>

#include <cassert>

namespace mbgl {

static SpriteLoaderObserver nullObserver;

struct SpriteLoader::Data {
    std::shared_ptr<const std::string> image;
    std::shared_ptr<const std::string> json;
    std::unique_ptr<AsyncRequest> jsonRequest;
    std::unique_ptr<AsyncRequest> spriteRequest;
};

SpriteLoader::SpriteLoader(float pixelRatio_)
    : pixelRatio(pixelRatio_), observer(&nullObserver), threadPool(Scheduler::GetBackground()) {}

SpriteLoader::~SpriteLoader() = default;

void SpriteLoader::load(const std::string& url, FileSource& fileSource) {
    if (url.empty()) {
        // Treat a non-existent sprite as a successfully loaded empty sprite.
        observer->onSpriteLoaded({});
        return;
    }

    data = std::make_unique<Data>();

    data->jsonRequest = fileSource.request(Resource::spriteJSON(url, pixelRatio), [this](Response res) {
        if (res.error) {
            observer->onSpriteError(std::make_exception_ptr(std::runtime_error(res.error->message)));
        } else if (res.notModified) {
            return;
        } else if (res.noContent) {
            data->json = std::make_shared<std::string>();
            emitSpriteLoadedIfComplete();
        } else {
            // Only trigger a sprite loaded event we got new data.
            assert(data->json != res.data);
            data->json = std::move(res.data);
            emitSpriteLoadedIfComplete();
        }
    });

    data->spriteRequest = fileSource.request(Resource::spriteImage(url, pixelRatio), [this](Response res) {
        if (res.error) {
            observer->onSpriteError(std::make_exception_ptr(std::runtime_error(res.error->message)));
        } else if (res.notModified) {
            return;
        } else if (res.noContent) {
            data->image = std::make_shared<std::string>();
            emitSpriteLoadedIfComplete();
        } else {
            assert(data->image != res.data);
            data->image = std::move(res.data);
            emitSpriteLoadedIfComplete();
        }
    });
}

void SpriteLoader::emitSpriteLoadedIfComplete() {
    assert(data);
    if (!data->image || !data->json) {
        return;
    }

    struct ParseResult {
        std::vector<Immutable<style::Image::Impl>> images;
        std::exception_ptr error;
    };

    auto parseClosure = [image = data->image, json = data->json]() -> ParseResult {
        try {
            return {parseSprite(*image, *json), nullptr};
        } catch (...) {
            return {{}, std::current_exception()};
        }
    };

    auto resultClosure = [this, weak = weakFactory.makeWeakPtr()](ParseResult result) {
        if (!weak) return; // This instance has been deleted.

        if (result.error) {
            observer->onSpriteError(result.error);
            return;
        }
        observer->onSpriteLoaded(std::move(result.images));
    };

    threadPool->scheduleAndReplyValue(parseClosure, resultClosure);
}

void SpriteLoader::setObserver(SpriteLoaderObserver* observer_) {
    observer = observer_;
}

} // namespace mbgl
