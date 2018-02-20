#include <mbgl/sprite/sprite_loader_worker.hpp>
#include <mbgl/sprite/sprite_loader.hpp>
#include <mbgl/sprite/sprite_parser.hpp>

namespace mbgl {

SpriteLoaderWorker::SpriteLoaderWorker(ActorRef<SpriteLoaderWorker>, ActorRef<SpriteLoader> parent_)
    : parent(std::move(parent_)) {
}

void SpriteLoaderWorker::parse(Blob image, Blob json) {
    try {
        if (!image) {
            // This shouldn't happen, since we always invoke it with a non-empty pointer.
            throw std::runtime_error("missing sprite image");
        }
        if (!json) {
            // This shouldn't happen, since we always invoke it with a non-empty pointer.
            throw std::runtime_error("missing sprite metadata");
        }

        parent.invoke(&SpriteLoader::onParsed, parseSprite(std::move(image), std::move(json)));
    } catch (...) {
        parent.invoke(&SpriteLoader::onError, std::current_exception());
    }
}

} // namespace mbgl
