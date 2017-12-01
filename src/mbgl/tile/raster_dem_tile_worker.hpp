#pragma once

#include <mbgl/actor/actor_ref.hpp>

#include <memory>
#include <string>

namespace mbgl {

class RasterDEMTile;

class RasterDEMTileWorker {
public:
    RasterDEMTileWorker(ActorRef<RasterDEMTileWorker>, ActorRef<RasterDEMTile>);

    void parse(std::shared_ptr<const std::string> data, uint64_t correlationID);

private:
    ActorRef<RasterDEMTile> parent;
};

} // namespace mbgl
