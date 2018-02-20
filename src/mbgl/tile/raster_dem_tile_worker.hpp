#pragma once

#include <mbgl/actor/actor_ref.hpp>
#include <mbgl/util/tileset.hpp>
#include <mbgl/util/blob.hpp>

#include <memory>
#include <string>

namespace mbgl {

class RasterDEMTile;

class RasterDEMTileWorker {
public:
    RasterDEMTileWorker(ActorRef<RasterDEMTileWorker>, ActorRef<RasterDEMTile>);

    void parse(Blob data, uint64_t correlationID, Tileset::DEMEncoding encoding);

private:
    ActorRef<RasterDEMTile> parent;
};

} // namespace mbgl
