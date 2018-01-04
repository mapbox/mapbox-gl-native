#pragma once

#include <mbgl/actor/actor_ref.hpp>
#include <mbgl/sprite/sprite_parser.hpp>

#include <memory>
#include <string>

namespace mbgl {

class SpriteLoader;

class SpriteLoaderWorker {
public:
    SpriteLoaderWorker(ActorRef<SpriteLoaderWorker>, ActorRef<SpriteLoader>);

    void parse(std::shared_ptr<const std::string> image, std::shared_ptr<const std::string> json);

private:
    ActorRef<SpriteLoader> parent;
};

} // namespace mbgl
