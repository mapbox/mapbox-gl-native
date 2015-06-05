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

bool LiveTileData::reparse(Worker&, std::function<void()> callback) {
    if (parsing.test_and_set(std::memory_order_acquire)) {
        return false;
    }

    workRequest = worker.send([this] {
        if (getState() != State::loaded) {
            return;
        }

        const LiveTile* tile = annotationManager.getTile(id);

        if (!tile) {
            state = State::parsed;
            return;
        }

        TileParseResult result;

        try {
            result = workerData.parse(*tile);
        } catch (const std::exception& ex) {
            std::stringstream message;
            message << "Failed to parse [" << int(id.sourceZ) << "/" << id.x << "/" << id.y << "]: " << ex.what();
            result = message.str();
        }

        if (getState() == TileData::State::obsolete) {
            return;
        } else if (result.is<State>()) {
            state = result.get<State>();
        } else {
            error = result.get<std::string>();
            state = State::obsolete;
        }

        parsing.clear(std::memory_order_release);
    }, callback);

    return true;
}
