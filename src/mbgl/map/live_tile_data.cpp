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
                           const LiveTile* tile_,
                           Style& style_,
                           const SourceInfo& source_,
                           std::function<void()> callback)
    : TileData(id_),
      worker(style_.workers),
      tileWorker(id_,
                 source_.source_id,
                 source_.max_zoom,
                 style_,
                 style_.layers,
                 state,
                 std::make_unique<CollisionTile>(0, 0, false)),
      tile(tile_) {
    state = State::loaded;

    if (!tile) {
        state = State::parsed;
        return;
    }

    reparse(callback);
}

bool LiveTileData::reparse(std::function<void()> callback) {
    if (parsing || (state != State::loaded && state != State::partial)) {
        return false;
    }

    parsing = true;

    workRequest = worker.parseLiveTile(tileWorker, *tile, [this, callback] (TileParseResult result) {
        parsing = false;

        if (result.is<State>()) {
            state = result.get<State>();
        } else {
            error = result.get<std::string>();
            state = State::obsolete;
        }

        callback();
    });

    return true;
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
