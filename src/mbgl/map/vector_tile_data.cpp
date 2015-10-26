#include <mbgl/map/vector_tile_data.hpp>
#include <mbgl/style/style_layer.hpp>
#include <mbgl/style/style_bucket.hpp>
#include <mbgl/map/source.hpp>
#include <mbgl/text/collision_tile.hpp>
#include <mbgl/storage/resource.hpp>
#include <mbgl/storage/response.hpp>
#include <mbgl/storage/file_source.hpp>
#include <mbgl/util/worker.hpp>
#include <mbgl/util/work_request.hpp>
#include <mbgl/style/style.hpp>

using namespace mbgl;

VectorTileData::VectorTileData(const TileID& id_,
                               Style& style_,
                               const SourceInfo& source_)
    : TileData(id_),
      worker(style_.workers),
      tileWorker(id_,
                 source_.source_id,
                 style_,
                 style_.layers,
                 state),
      source(source_) {
}

VectorTileData::~VectorTileData() {
    cancel();
}

void VectorTileData::request(float pixelRatio, const std::function<void()>& callback) {
    std::string url = source.tileURL(id, pixelRatio);
    state = State::loading;

    FileSource* fs = util::ThreadContext::getFileSource();
    req = fs->request({ Resource::Kind::Tile, url }, util::RunLoop::getLoop(), [url, callback, this](const Response &res) {
        // Do not cancel the request here; we want to get notified about tiles that expire.

        if (res.data && data == res.data) {
            // We got the same data again. Abort early.
            return;
        }

        if (res.status == Response::NotFound) {
            state = State::parsed;
            callback();
            return;
        }

        if (res.status != Response::Successful) {
            std::stringstream message;
            message << "Failed to load [" << url << "]: " << res.message;
            error = message.str();
            state = State::obsolete;
            callback();
            return;
        }

        if (state == State::loading) {
            state = State::loaded;
        } else if (isReady()) {
            state = State::partial;
        }
        data = res.data;

        parse(callback);
    });
}

void VectorTileData::parse(std::function<void ()> callback) {
    // Kick off a fresh parse of this tile. This happens when the tile is new, or
    // when tile data changed. Replacing the workdRequest will cancel a pending work
    // request in case there is one.
    workRequest.reset();
    workRequest = worker.parseVectorTile(tileWorker, data, targetConfig, [this, callback, config = targetConfig] (TileParseResult result) {
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
}

bool VectorTileData::parsePending(std::function<void()> callback) {
    if (workRequest) {
        // There's already parsing or placement going on.
        return false;
    }

    workRequest.reset();
    workRequest = worker.parsePendingVectorTileLayers(tileWorker, [this, callback] (TileParseResult result) {
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
