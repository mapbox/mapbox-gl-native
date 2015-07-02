#include <mbgl/map/annotation.hpp>
#include <mbgl/map/live_tile_data.hpp>
#include <mbgl/map/live_tile.hpp>
#include <mbgl/style/style_layer.hpp>
#include <mbgl/map/source.hpp>
#include <mbgl/text/collision_tile.hpp>
#include <mbgl/util/worker.hpp>
#include <mbgl/util/work_request.hpp>
#include <mbgl/style/style.hpp>

#include <sstream>

using namespace mbgl;

LiveTileData::LiveTileData(const TileID& id_,
                           const LiveTile* tile,
                           Style& style_,
                           const SourceInfo& source_,
                           std::function<void()> callback)
    : TileData(id_),
      worker(style_.workers),
      tileWorker(id_,
                 style_,
                 style_.layers,
                 source_.max_zoom,
                 state,
                 std::make_unique<CollisionTile>(id_.z, 4096,
                                    source_.tile_size * id.overscaling,
                                    0, false)) {
    state = State::loaded;

    if (!tile) {
        state = State::parsed;
        return;
    }

    workRequest = worker.parseLiveTile(tileWorker, *tile, [this, callback] (TileParseResult result) {
        if (result.is<State>()) {
            state = result.get<State>();
        } else {
            error = result.get<std::string>();
            state = State::obsolete;
        }

        callback();
    });
}

LiveTileData::~LiveTileData() {
    cancel();
}

Bucket* LiveTileData::getBucket(const StyleLayer& layer) {
    if (!isReady() || !layer.bucket) {
        return nullptr;
    }

    return tileWorker.getBucket(layer);
}

void LiveTileData::cancel() {
    state = State::obsolete;
    workRequest.reset();
}
