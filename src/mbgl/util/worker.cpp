#include <mbgl/util/worker.hpp>
#include <mbgl/util/work_task.hpp>
#include <mbgl/util/work_request.hpp>
#include <mbgl/platform/platform.hpp>
#include <mbgl/map/vector_tile.hpp>
#include <mbgl/annotation/annotation_tile.hpp>
#include <mbgl/util/pbf.hpp>
#include <mbgl/renderer/raster_bucket.hpp>

#include <cassert>
#include <future>

namespace mbgl {

class Worker::Impl {
public:
    Impl() = default;

    void parseRasterTile(std::unique_ptr<RasterBucket> bucket,
                         const std::shared_ptr<const std::string> data,
                         std::function<void(TileParseResult)> callback) {
        std::unique_ptr<util::Image> image(new util::Image(*data));
        if (!(*image)) {
            callback(TileParseResult("error parsing raster image"));
        }

        if (!bucket->setImage(std::move(image))) {
            callback(TileParseResult("error setting raster image to bucket"));
        }

        TileParseResultBuckets result;
        result.buckets.emplace_back("raster", std::move(bucket));
        result.state = TileData::State::parsed;

        callback(std::move(result));
    }

    void parseVectorTile(TileWorker* worker,
                         const std::shared_ptr<const std::string> data,
                         std::function<void(TileParseResult)> callback) {
        try {
            pbf tilePBF(reinterpret_cast<const unsigned char*>(data->data()), data->size());
            callback(worker->parseAllLayers(VectorTile(tilePBF)));
        } catch (const std::exception& ex) {
            callback(TileParseResult(ex.what()));
        }
    }

    void parsePendingVectorTileLayers(TileWorker* worker,
                                      std::function<void(TileParseResult)> callback) {
        try {
            callback(worker->parsePendingLayers());
        } catch (const std::exception& ex) {
            callback(TileParseResult(ex.what()));
        }
    }

    void parseLiveTile(TileWorker* worker,
                       const AnnotationTile* tile,
                       std::function<void(TileParseResult)> callback) {
        try {
            callback(worker->parseAllLayers(*tile));
        } catch (const std::exception& ex) {
            callback(TileParseResult(ex.what()));
        }
    }

    void redoPlacement(TileWorker* worker,
                       const std::unordered_map<std::string, std::unique_ptr<Bucket>>* buckets,
                       float angle,
                       float pitch,
                       bool collisionDebug,
                       std::function<void()> callback) {
        worker->redoPlacement(buckets, angle, pitch, collisionDebug);
        callback();
    }
};

Worker::Worker(std::size_t count) {
    util::ThreadContext context = { "Worker", util::ThreadType::Worker, util::ThreadPriority::Low };
    for (std::size_t i = 0; i < count; i++) {
        threads.emplace_back(std::make_unique<util::Thread<Impl>>(context));
    }
}

Worker::~Worker() = default;

std::unique_ptr<WorkRequest>
Worker::parseRasterTile(std::unique_ptr<RasterBucket> bucket,
                        const std::shared_ptr<const std::string> data,
                        std::function<void(TileParseResult)> callback) {
    current = (current + 1) % threads.size();
    return threads[current]->invokeWithCallback(&Worker::Impl::parseRasterTile, callback, bucket,
                                                data);
}

std::unique_ptr<WorkRequest>
Worker::parseVectorTile(TileWorker& worker,
                        const std::shared_ptr<const std::string> data,
                        std::function<void(TileParseResult)> callback) {
    current = (current + 1) % threads.size();
    return threads[current]->invokeWithCallback(&Worker::Impl::parseVectorTile, callback, &worker,
                                                data);
}

std::unique_ptr<WorkRequest>
Worker::parsePendingVectorTileLayers(TileWorker& worker,
                                     std::function<void(TileParseResult)> callback) {
    current = (current + 1) % threads.size();
    return threads[current]->invokeWithCallback(&Worker::Impl::parsePendingVectorTileLayers,
                                                callback, &worker);
}

std::unique_ptr<WorkRequest> Worker::parseLiveTile(TileWorker& worker,
                                                   const AnnotationTile& tile,
                                                   std::function<void(TileParseResult)> callback) {
    current = (current + 1) % threads.size();
    return threads[current]->invokeWithCallback(&Worker::Impl::parseLiveTile, callback, &worker,
                                                &tile);
}

std::unique_ptr<WorkRequest>
Worker::redoPlacement(TileWorker& worker,
                      const std::unordered_map<std::string, std::unique_ptr<Bucket>>& buckets,
                      float angle,
                      float pitch,
                      bool collisionDebug,
                      std::function<void()> callback) {
    current = (current + 1) % threads.size();
    return threads[current]->invokeWithCallback(&Worker::Impl::redoPlacement, callback, &worker,
                                                &buckets, angle, pitch, collisionDebug);
}

} // end namespace mbgl
