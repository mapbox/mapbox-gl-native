#include <mbgl/map/vector_tile_data.hpp>
#include <mbgl/map/geometry_tile.hpp>
#include <mbgl/style/style_layer.hpp>
#include <mbgl/style/style_bucket.hpp>
#include <mbgl/util/worker.hpp>
#include <mbgl/util/work_request.hpp>
#include <mbgl/style/style.hpp>

#include <sstream>

namespace mbgl {

VectorTileData::VectorTileData(const TileID& id_,
                               std::unique_ptr<GeometryTileMonitor> monitor_,
                               std::string sourceID,
                               Style& style_,
                               const std::function<void()>& callback)
    : TileData(id_),
      worker(style_.workers),
      tileWorker(id_,
                 sourceID,
                 style_,
                 style_.layers,
                 state),
      monitor(std::move(monitor_))
{
    state = State::loading;
    req = monitor->monitorTile([callback, sourceID, this](std::exception_ptr err, std::unique_ptr<GeometryTile> tile) {
        if (err) {
            try {
                std::rethrow_exception(err);
            } catch (const std::exception& e) {
                error = e.what();
                state = State::obsolete;
                callback();
                return;
            }
        }

        if (!tile) {
            state = State::parsed;
            buckets.clear();
            callback();
            return;
        }

        if (state == State::loading) {
            state = State::loaded;
        } else if (isReady()) {
            state = State::partial;
        }

        // Kick off a fresh parse of this tile. This happens when the tile is new, or
        // when tile data changed. Replacing the workdRequest will cancel a pending work
        // request in case there is one.
        workRequest.reset();
        workRequest = worker.parseGeometryTile(tileWorker, std::move(tile), targetConfig, [callback, sourceID, this, config = targetConfig] (TileParseResult result) {
            workRequest.reset();
            if (state == State::obsolete) {
                return;
            }

            if (result.is<TileParseResultBuckets>()) {
                auto& resultBuckets = result.get<TileParseResultBuckets>();
                state = resultBuckets.state;

                // Persist the configuration we just placed so that we can later check whether we need to
                // place again in case the configuration has changed.
                placedConfig = config;

                // Move over all buckets we received in this parse request, potentially overwriting
                // existing buckets in case we got a refresh parse.
                buckets = std::move(resultBuckets.buckets);

                // The target configuration could have changed since we started placement. In this case,
                // we're starting another placement run.
                if (placedConfig != targetConfig) {
                    redoPlacement();
                }
            } else {
                std::stringstream message;
                message << "Failed to parse [" << std::string(id) << "]: " << result.get<std::string>();
                error = message.str();
                state = State::obsolete;
            }

            callback();
        });
    });
}

VectorTileData::~VectorTileData() {
    cancel();
}

bool VectorTileData::parsePending(std::function<void()> callback) {
    if (workRequest) {
        // There's already parsing or placement going on.
        return false;
    }

    workRequest.reset();
    workRequest = worker.parsePendingGeometryTileLayers(tileWorker, [this, callback] (TileParseResult result) {
        workRequest.reset();
        if (state == State::obsolete) {
            return;
        }

        if (result.is<TileParseResultBuckets>()) {
            auto& resultBuckets = result.get<TileParseResultBuckets>();
            state = resultBuckets.state;

            // Move over all buckets we received in this parse request, potentially overwriting
            // existing buckets in case we got a refresh parse.
            for (auto& bucket : resultBuckets.buckets) {
                buckets[bucket.first] = std::move(bucket.second);
            }

            // The target configuration could have changed since we started placement. In this case,
            // we're starting another placement run.
            if (placedConfig != targetConfig) {
                redoPlacement();
            }
        } else {
            std::stringstream message;
            message << "Failed to parse [" << std::string(id) << "]: " << result.get<std::string>();
            error = message.str();
            state = State::obsolete;
        }

        callback();
    });

    return true;
}

Bucket* VectorTileData::getBucket(const StyleLayer& layer) {
    if (!layer.bucket) {
        return nullptr;
    }

    const auto it = buckets.find(layer.bucket->name);
    if (it == buckets.end()) {
        return nullptr;
    }

    assert(it->second);
    return it->second.get();
}

void VectorTileData::redoPlacement(const PlacementConfig newConfig) {
    if (newConfig != placedConfig) {
        targetConfig = newConfig;

        if (!workRequest) {
            // Don't start a new placement request when the current one hasn't completed yet, or when
            // we are parsing buckets.
            redoPlacement();
        }
    }
}

void VectorTileData::redoPlacement() {
    workRequest.reset();
    workRequest = worker.redoPlacement(tileWorker, buckets, targetConfig, [this, config = targetConfig] {
        workRequest.reset();

        // Persist the configuration we just placed so that we can later check whether we need to
        // place again in case the configuration has changed.
        placedConfig = config;

        for (auto& bucket : buckets) {
            bucket.second->swapRenderData();
        }

        // The target configuration could have changed since we started placement. In this case,
        // we're starting another placement run.
        if (placedConfig != targetConfig) {
            redoPlacement();
        }
    });
}

void VectorTileData::cancel() {
    if (state != State::obsolete) {
        state = State::obsolete;
    }
    req = nullptr;
    workRequest.reset();
}

}
