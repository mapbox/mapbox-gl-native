#include <mbgl/sprite/sprite_atlas_worker.hpp>
#include <mbgl/sprite/sprite_atlas.hpp>
#include <mbgl/sprite/sprite_parser.hpp>

namespace mbgl {

SpriteAtlasWorker::SpriteAtlasWorker(ActorRef<SpriteAtlasWorker>, ActorRef<SpriteAtlas> parent_)
    : parent(std::move(parent_)) {
}

void SpriteAtlasWorker::parse(std::shared_ptr<const std::string> image,
                              std::shared_ptr<const std::string> json) {
    try {
        if (!image) {
            // This shouldn't happen, since we always invoke it with a non-empty pointer.
            throw std::runtime_error("missing sprite image");
        }
        if (!json) {
            // This shouldn't happen, since we always invoke it with a non-empty pointer.
            throw std::runtime_error("missing sprite metadata");
        }

        parent.invoke(&SpriteAtlas::onParsed, parseSprite(*image, *json));
    } catch (...) {
        parent.invoke(&SpriteAtlas::onError, std::current_exception());
    }
}

} // namespace mbgl
