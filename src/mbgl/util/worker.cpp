#include <mbgl/util/worker.hpp>
#include <mbgl/util/work_task.hpp>
#include <mbgl/util/work_request.hpp>
#include <mbgl/platform/platform.hpp>
#include <mbgl/renderer/raster_bucket.hpp>
#include <mbgl/map/geometry_tile.hpp>

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

    void parseGeometryTile(TileWorker* worker,
                           std::unique_ptr<GeometryTile> tile,
                           PlacementConfig config,
                           std::function<void(TileParseResult)> callback) {
        try {
            callback(worker->parseAllLayers(*tile, config));
        } catch (const std::exception& ex) {
            callback(TileParseResult(ex.what()));
        }
    }

    void parsePendingGeometryTileLayers(TileWorker* worker,
                                        std::function<void(TileParseResult)> callback) {
        try {
            callback(worker->parsePendingLayers());
        } catch (const std::exception& ex) {
            callback(TileParseResult(ex.what()));
        }
    }

    void redoPlacement(TileWorker* worker,
                       const std::unordered_map<std::string, std::unique_ptr<Bucket>>* buckets,
                       PlacementConfig config,
                       std::function<void()> callback) {
        worker->redoPlacement(buckets, config);
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
Worker::parseGeometryTile(TileWorker& worker,
                          std::unique_ptr<GeometryTile> tile,
                          PlacementConfig config,
                          std::function<void(TileParseResult)> callback) {
    current = (current + 1) % threads.size();
    return threads[current]->invokeWithCallback(&Worker::Impl::parseGeometryTile, callback, &worker,
                                                std::move(tile), config);
}

std::unique_ptr<WorkRequest>
Worker::parsePendingGeometryTileLayers(TileWorker& worker,
                                       std::function<void(TileParseResult)> callback) {
    current = (current + 1) % threads.size();
    return threads[current]->invokeWithCallback(&Worker::Impl::parsePendingGeometryTileLayers,
                                                callback, &worker);
}

std::unique_ptr<WorkRequest>
Worker::redoPlacement(TileWorker& worker,
                      const std::unordered_map<std::string, std::unique_ptr<Bucket>>& buckets,
                      PlacementConfig config,
                      std::function<void()> callback) {
    current = (current + 1) % threads.size();
    return threads[current]->invokeWithCallback(&Worker::Impl::redoPlacement, callback, &worker,
                                                &buckets, config);
}

} // end namespace mbgl
