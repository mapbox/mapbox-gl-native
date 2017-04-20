#pragma once

#include <mbgl/actor/actor_ref.hpp>
#include <mbgl/sprite/sprite_parser.hpp>

#include <memory>
#include <string>

namespace mbgl {

class SpriteAtlas;

class SpriteAtlasWorker {
public:
    SpriteAtlasWorker(ActorRef<SpriteAtlasWorker>, ActorRef<SpriteAtlas>);

    void parse(std::shared_ptr<const std::string> image, std::shared_ptr<const std::string> json);

private:
    ActorRef<SpriteAtlas> parent;
};

} // namespace mbgl
