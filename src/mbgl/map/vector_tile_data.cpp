#include <mbgl/map/vector_tile_data.hpp>
#include <mbgl/map/geometry_tile.hpp>
#include <mbgl/style/style_layer.hpp>
#include <mbgl/util/worker.hpp>
#include <mbgl/util/work_request.hpp>
#include <mbgl/style/style.hpp>
#include <mbgl/storage/file_source.hpp>

namespace mbgl {

VectorTileData::VectorTileData(const TileID& id_,
                               std::unique_ptr<GeometryTileMonitor> monitor_,
                               std::string sourceID,
                               Style& style_,
                               const MapMode mode_,
                               const std::function<void(std::exception_ptr)>& callback)
    : TileData(id_),
      style(style_),
      worker(style_.workers),
      tileWorker(id_,
                 sourceID,
                 *style_.spriteStore,
                 *style_.glyphAtlas,
                 *style_.glyphStore,
                 state,
                 mode_),
      monitor(std::move(monitor_))
{
    state = State::loading;
    tileRequest = monitor->monitorTile([callback, this](std::exception_ptr err,
                                                        std::unique_ptr<GeometryTile> tile,
                                                        optional<SystemTimePoint> modified_,
                                                        optional<SystemTimePoint> expires_) {
        if (err) {
            callback(err);
            return;
        }

        modified = modified_;
        expires = expires_;

        if (!tile) {
            // This is a 404 response. We're treating these as empty tiles.
            workRequest.reset();
            state = State::parsed;
            buckets.clear();
            callback(err);
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
        workRequest = worker.parseGeometryTile(tileWorker, style.getLayers(), std::move(tile), targetConfig, [callback, this, config = targetConfig] (TileParseResult result) {
            workRequest.reset();
            if (state == State::obsolete) {
                return;
            }

            std::exception_ptr error;
            if (result.is<TileParseResultBuckets>()) {
                auto& resultBuckets = result.get<TileParseResultBuckets>();
                state = resultBuckets.state;

                // Persist the configuration we just placed so that we can later check whether we need to
                // place again in case the configuration has changed.
                placedConfig = config;

                // Move over all buckets we received in this parse request, potentially overwriting
                // existing buckets in case we got a refresh parse.
                buckets = std::move(resultBuckets.buckets);

            } else {
                error = result.get<std::exception_ptr>();
                state = State::obsolete;
            }

            callback(error);
        });
    });
}

VectorTileData::~VectorTileData() {
    cancel();
}

bool VectorTileData::parsePending(std::function<void(std::exception_ptr)> callback) {
    if (workRequest) {
        // There's already parsing or placement going on.
        return false;
    }

    workRequest.reset();
    workRequest = worker.parsePendingGeometryTileLayers(tileWorker, targetConfig, [this, callback, config = targetConfig] (TileParseResult result) {
        workRequest.reset();
        if (state == State::obsolete) {
            return;
        }

        std::exception_ptr error;
        if (result.is<TileParseResultBuckets>()) {
            auto& resultBuckets = result.get<TileParseResultBuckets>();
            state = resultBuckets.state;

            // Move over all buckets we received in this parse request, potentially overwriting
            // existing buckets in case we got a refresh parse.
            for (auto& bucket : resultBuckets.buckets) {
                buckets[bucket.first] = std::move(bucket.second);
            }

            // Persist the configuration we just placed so that we can later check whether we need to
            // place again in case the configuration has changed.
            placedConfig = config;

        } else {
            error = result.get<std::exception_ptr>();
            state = State::obsolete;
        }

        callback(error);
    });

    return true;
}

Bucket* VectorTileData::getBucket(const StyleLayer& layer) {
    const auto it = buckets.find(layer.bucketName());
    if (it == buckets.end()) {
        return nullptr;
    }

    assert(it->second);
    return it->second.get();
}

void VectorTileData::redoPlacement(const PlacementConfig newConfig, const std::function<void()>& callback) {
    if (newConfig != placedConfig) {
        targetConfig = newConfig;

        if (!workRequest) {
            // Don't start a new placement request when the current one hasn't completed yet, or when
            // we are parsing buckets.
            redoPlacement(callback);
        }
    }
}

void VectorTileData::redoPlacement(const std::function<void()>& callback) {
    workRequest.reset();
    workRequest = worker.redoPlacement(tileWorker, buckets, targetConfig, [this, callback, config = targetConfig] {
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
            redoPlacement(callback);
        } else {
            callback();
        }
    });
}

void VectorTileData::cancel() {
    state = State::obsolete;
    tileRequest.reset();
    workRequest.reset();
}

} // namespace mbgl
