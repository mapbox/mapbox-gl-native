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
                           Style& style_,
                           const SourceInfo& source_,
                           std::function<void()> callback)
    : TileData(id_),
      worker(style_.workers),
      tileWorker(id_,
                 source_.source_id,
                 style_,
                 style_.layers,
                 state,
                 std::make_unique<CollisionTile>(0, 0, false)),
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

    workRequest = worker.parseLiveTile(tileWorker, *tile, [this, callback] (TileParseResult result) {
        workRequest.reset();

        if (result.is<TileParseResultBuckets>()) {
            auto& resultBuckets = result.get<TileParseResultBuckets>();
            state = resultBuckets.state;

            // Move over all buckets we received in this parse request, potentially overwriting
            // existing buckets in case we got a refresh parse.
            for (auto& bucket : resultBuckets.buckets) {
                buckets[bucket.first] = std::move(bucket.second);
            }

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
