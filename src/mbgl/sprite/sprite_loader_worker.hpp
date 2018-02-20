#pragma once

#include <mbgl/actor/actor_ref.hpp>
#include <mbgl/sprite/sprite_parser.hpp>
#include <mbgl/storage/response.hpp>

#include <memory>
#include <string>

namespace mbgl {

class SpriteLoader;

class SpriteLoaderWorker {
public:
    SpriteLoaderWorker(ActorRef<SpriteLoaderWorker>, ActorRef<SpriteLoader>);

    void parse(Blob image, Blob json);

private:
    ActorRef<SpriteLoader> parent;
};

} // namespace mbgl
