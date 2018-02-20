#pragma once

#include <mbgl/actor/actor_ref.hpp>
#include <mbgl/util/blob.hpp>

#include <memory>
#include <string>

namespace mbgl {

class RasterTile;

class RasterTileWorker {
public:
    RasterTileWorker(ActorRef<RasterTileWorker>, ActorRef<RasterTile>);

    void parse(Blob data, uint64_t correlationID);

private:
    ActorRef<RasterTile> parent;
};

} // namespace mbgl
