#pragma once

#include <mbgl/actor/actor_ref.hpp>

#include <memory>
#include <string>

namespace mbgl {

class RasterTile;

class RasterTileWorker {
public:
    RasterTileWorker(const ActorRef<RasterTileWorker>&, ActorRef<RasterTile>);

    void parse(const std::shared_ptr<const std::string>& data, uint64_t correlationID);

private:
    ActorRef<RasterTile> parent;
};

} // namespace mbgl
