#include <mbgl/map/live_tile_data.hpp>
#include <mbgl/annotation/annotation_tile.hpp>
#include <mbgl/style/style_layer.hpp>
#include <mbgl/map/source.hpp>
#include <mbgl/text/collision_tile.hpp>
#include <mbgl/util/worker.hpp>
#include <mbgl/util/work_request.hpp>
#include <mbgl/style/style.hpp>
#include <mbgl/style/style_bucket.hpp>

#include <sstream>

using namespace mbgl;

LiveTileData::LiveTileData(const TileID& id_,
                           std::unique_ptr<AnnotationTile> tile_,
                           Style& style,
                           const SourceInfo& source,
                           std::function<void()> callback)
    : TileData(id_),
      worker(style.workers),
      tileWorker(id, source.source_id, style, style.layers, state),
      tile(std::move(tile_)) {
    state = State::loaded;

    if (!tile) {
        state = State::parsed;
        return;
    }

    parsePending(callback);
}

bool LiveTileData::parsePending(std::function<void()> callback) {
    if (workRequest || (state != State::loaded && state != State::partial)) {
        return false;
    }

    workRequest.reset();
    workRequest = worker.parseLiveTile(tileWorker, *tile, targetConfig, [this, callback, config = targetConfig] (TileParseResult result) {
        workRequest.reset();

        if (result.is<TileParseResultBuckets>()) {
            auto& resultBuckets = result.get<TileParseResultBuckets>();
            state = resultBuckets.state;

            // Persist the configuration we just placed so that we can later check whether we need
            // to place again in case the configuration has changed.
            placedConfig = config;

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
            error = result.get<std::string>();
            state = State::obsolete;
        }

        callback();

        // The target configuration could have changed since we started placement. In this case,
        // we're starting another placement run.
        if (!workRequest && placedConfig != targetConfig) {
            redoPlacement();
        }
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

    const auto it = buckets.find(layer.bucket->name);
    if (it == buckets.end()) {
        return nullptr;
    }

    assert(it->second);
    return it->second.get();
}

void LiveTileData::cancel() {
    state = State::obsolete;
    workRequest.reset();
}

void LiveTileData::redoPlacement(const PlacementConfig newConfig) {
    if (newConfig != placedConfig) {
        targetConfig = newConfig;

        if (!workRequest) {
            // Don't start a new placement request when the current one hasn't completed yet, or
            // when we are parsing buckets.
            redoPlacement();
        }
    }
}

void LiveTileData::redoPlacement() {
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
