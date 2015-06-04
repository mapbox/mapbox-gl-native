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
    : VectorTileData::VectorTileData(id_, style_, source_, angle_, collisionDebug_),
      annotationManager(annotationManager_) {
    // live features are always ready
    setState(State::loaded);
}

LiveTileData::~LiveTileData() {
    // Cancel in most derived class destructor so that worker tasks are joined before
    // any member data goes away.
    cancel();
}

bool LiveTileData::reparse(Worker&, std::function<void()> callback) {
    if (!mayStartParsing()) {
        return false;
    }

    workRequest = worker.send([this] {
        if (getState() != State::loaded) {
            return;
        }

        const LiveTile* tile = annotationManager.getTile(id);

        if (!tile) {
            setState(State::parsed);
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
            setState(result.get<State>());
        } else {
            setError(result.get<std::string>());
        }

        endParsing();
    }, callback);

    return true;
}
