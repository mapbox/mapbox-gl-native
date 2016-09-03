#include <mbgl/util/worker.hpp>
#include <mbgl/util/work_task.hpp>
#include <mbgl/util/work_request.hpp>
#include <mbgl/platform/platform.hpp>
#include <mbgl/renderer/raster_bucket.hpp>
#include <mbgl/tile/geometry_tile_data.hpp>
#include <mbgl/style/layer.hpp>
#include <mbgl/text/collision_tile.hpp>

#include <cassert>
#include <future>

namespace mbgl {

class Worker::Impl {
public:
    Impl() = default;

    void parseRasterTile(std::unique_ptr<RasterBucket> bucket,
                         std::shared_ptr<const std::string> data,
                         std::function<void(RasterTileParseResult)> callback) {
        try {
            bucket->setImage(decodeImage(*data));
            // Destruct the shared pointer before calling the callback.
            data.reset();
            callback(RasterTileParseResult(std::move(bucket)));
        } catch (...) {
            callback(std::current_exception());
        }
    }

    void parseGeometryTile(TileWorker* worker,
                           std::vector<std::unique_ptr<style::Layer>> layers,
                           std::unique_ptr<const GeometryTileData> tileData,
                           PlacementConfig config,
                           std::function<void(TileParseResult)> callback) {
        try {
            callback(worker->parseAllLayers(std::move(layers), std::move(tileData), config));
        } catch (...) {
            callback(std::current_exception());
        }
    }

    void parsePendingGeometryTileLayers(TileWorker* worker,
                                        PlacementConfig config,
                                        std::function<void(TileParseResult)> callback) {
        try {
            callback(worker->parsePendingLayers(config));
        } catch (...) {
            callback(std::current_exception());
        }
    }

    void redoLayout(TileWorker* worker,
                    std::vector<std::unique_ptr<style::Layer>> layers,
                    PlacementConfig config,
                    std::function<void(TileParseResult)> callback) {
        try {
            callback(worker->redoLayout(std::move(layers), config));
        } catch (...) {
            callback(std::current_exception());
        }
    }

    void redoPlacement(TileWorker* worker,
                       const std::unordered_map<std::string, std::unique_ptr<Bucket>>* buckets,
                       PlacementConfig config,
                       std::function<void(std::unique_ptr<CollisionTile>)> callback) {
        callback(worker->redoPlacement(buckets, config));
    }
};

Worker::Worker(std::size_t count) {
    util::ThreadContext context = { "Worker", util::ThreadPriority::Low };
    for (std::size_t i = 0; i < count; i++) {
        threads.emplace_back(std::make_unique<util::Thread<Impl>>(context));
    }
}

Worker::~Worker() = default;

std::unique_ptr<AsyncRequest>
Worker::parseRasterTile(std::unique_ptr<RasterBucket> bucket,
                        const std::shared_ptr<const std::string> data,
                        std::function<void(RasterTileParseResult)> callback) {
    current = (current + 1) % threads.size();
    return threads[current]->invokeWithCallback(&Worker::Impl::parseRasterTile, bucket,
                                                data, callback);
}

std::unique_ptr<AsyncRequest>
Worker::parseGeometryTile(TileWorker& worker,
                          std::vector<std::unique_ptr<style::Layer>> layers,
                          std::unique_ptr<const GeometryTileData> tileData,
                          PlacementConfig config,
                          std::function<void(TileParseResult)> callback) {
    current = (current + 1) % threads.size();
    return threads[current]->invokeWithCallback(&Worker::Impl::parseGeometryTile, &worker,
                                                std::move(layers), std::move(tileData), config, callback);
}

std::unique_ptr<AsyncRequest>
Worker::parsePendingGeometryTileLayers(TileWorker& worker,
                                       PlacementConfig config,
                                       std::function<void(TileParseResult)> callback) {
    current = (current + 1) % threads.size();
    return threads[current]->invokeWithCallback(&Worker::Impl::parsePendingGeometryTileLayers,
                                                &worker, config, callback);
}

std::unique_ptr<AsyncRequest>
Worker::redoLayout(TileWorker& worker,
                   std::vector<std::unique_ptr<style::Layer>> layers,
                   PlacementConfig config,
                   std::function<void(TileParseResult)> callback) {
    current = (current + 1) % threads.size();
    return threads[current]->invokeWithCallback(&Worker::Impl::redoLayout, &worker,
                                                std::move(layers), config, callback);
}

std::unique_ptr<AsyncRequest>
Worker::redoPlacement(TileWorker& worker,
                      const std::unordered_map<std::string, std::unique_ptr<Bucket>>& buckets,
                      PlacementConfig config,
                      std::function<void(std::unique_ptr<CollisionTile>)> callback) {
    current = (current + 1) % threads.size();
    return threads[current]->invokeWithCallback(&Worker::Impl::redoPlacement, &worker,
                                                &buckets, config, callback);
}

} // end namespace mbgl
