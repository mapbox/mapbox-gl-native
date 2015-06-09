#include <mbgl/map/annotation.hpp>
#include <mbgl/map/live_tile_data.hpp>
#include <mbgl/map/live_tile.hpp>
#include <mbgl/util/worker.hpp>
#include <mbgl/util/work_request.hpp>

#include <sstream>

using namespace mbgl;

LiveTileData::LiveTileData(const TileID& id_,
                           AnnotationManager& annotationManager_,
                           Style& style_,
                           const SourceInfo& source_,
                           float angle_,
                           bool collisionDebug_)
    : VectorTileData(id_, style_, source_, angle_, collisionDebug_),
      annotationManager(annotationManager_) {
    // live features are always loaded
    state = State::loaded;
}

LiveTileData::~LiveTileData() {
    cancel();
}

bool LiveTileData::reparse(Worker& worker, std::function<void()> callback) {
    if (parsing || (state != State::loaded && state != State::partial)) {
        return false;
    }

    const LiveTile* tile = annotationManager.getTile(id);
    if (!tile) {
        state = State::parsed;
        return true;
    }

    parsing = true;

    workRequest = worker.parseLiveTile(tileWorker, *tile, [this, callback] (TileParseResult result) {
        parsing = false;

        if (state == State::obsolete) {
            return;
        }

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
