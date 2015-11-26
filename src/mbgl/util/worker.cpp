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
                         std::function<void(RasterTileParseResult)> callback) {
        PremultipliedImage image;

        try {
            image = decodeImage(*data);
        } catch (...) {
            callback(RasterTileParseResult("error parsing raster image"));
        }

        if (!bucket->setImage(std::move(image))) {
            callback(RasterTileParseResult("error setting raster image to bucket"));
        }

        callback(RasterTileParseResult(std::move(bucket)));
    }

    void parseGeometryTile(TileWorker* worker,
                           std::vector<util::ptr<StyleLayer>> layers,
                           std::unique_ptr<GeometryTile> tile,
                           PlacementConfig config,
                           std::function<void(TileParseResult)> callback) {
        try {
            callback(worker->parseAllLayers(layers, *tile, config));
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
                       std::vector<util::ptr<StyleLayer>> layers,
                       const std::unordered_map<std::string, std::unique_ptr<Bucket>>* buckets,
                       PlacementConfig config,
                       std::function<void()> callback) {
        worker->redoPlacement(layers, buckets, config);
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
                        std::function<void(RasterTileParseResult)> callback) {
    current = (current + 1) % threads.size();
    return threads[current]->invokeWithCallback(&Worker::Impl::parseRasterTile, callback, bucket,
                                                data);
}

std::unique_ptr<WorkRequest>
Worker::parseGeometryTile(TileWorker& worker,
                          std::vector<util::ptr<StyleLayer>> layers,
                          std::unique_ptr<GeometryTile> tile,
                          PlacementConfig config,
                          std::function<void(TileParseResult)> callback) {
    current = (current + 1) % threads.size();
    return threads[current]->invokeWithCallback(&Worker::Impl::parseGeometryTile, callback, &worker,
                                                std::move(layers), std::move(tile), config);
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
                      std::vector<util::ptr<StyleLayer>> layers,
                      const std::unordered_map<std::string, std::unique_ptr<Bucket>>& buckets,
                      PlacementConfig config,
                      std::function<void()> callback) {
    current = (current + 1) % threads.size();
    return threads[current]->invokeWithCallback(&Worker::Impl::redoPlacement, callback, &worker,
                                                layers, &buckets, config);
}

} // end namespace mbgl
