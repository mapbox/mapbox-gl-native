#pragma once

#include <mbgl/util/noncopyable.hpp>
#include <mbgl/util/thread.hpp>
#include <mbgl/tile/tile_worker.hpp>

#include <functional>
#include <memory>

namespace mbgl {

class AsyncRequest;
class RasterBucket;
class GeometryTileLoader;
class CollisionTile;

using RasterTileParseResult = variant<
    std::unique_ptr<Bucket>, // success
    std::exception_ptr>;     // error

class Worker : public mbgl::util::noncopyable {
public:
    explicit Worker(std::size_t count);
    ~Worker();

    // Request work be done on a thread pool. Callbacks are executed on the invoking
    // thread, which must have a run loop, after the work is complete.
    //
    // The return value represents the request to perform the work asynchronously.
    // Its destructor guarantees that the work function has finished executing, and
    // that the after function has either finished executing or will not execute.
    // Together, this means that an object may make a work request with lambdas which
    // bind references to itself, and if and when those lambdas execute, the references
    // will still be valid.

    using Request = std::unique_ptr<AsyncRequest>;

    Request parseRasterTile(std::unique_ptr<RasterBucket> bucket,
                            std::shared_ptr<const std::string> data,
                            std::function<void(RasterTileParseResult)> callback);

    Request parseGeometryTile(TileWorker&,
                              std::vector<std::unique_ptr<style::Layer>>,
                              std::unique_ptr<const GeometryTileData>,
                              PlacementConfig,
                              std::function<void(TileParseResult)> callback);

    Request parsePendingGeometryTileLayers(TileWorker&,
                                           PlacementConfig config,
                                           std::function<void(TileParseResult)> callback);

    Request redoPlacement(TileWorker&,
                          const std::unordered_map<std::string, std::unique_ptr<Bucket>>&,
                          PlacementConfig config,
                          std::function<void(std::unique_ptr<CollisionTile>)> callback);

private:
    class Impl;
    std::vector<std::unique_ptr<util::Thread<Impl>>> threads;
    std::size_t current = 0;
};
} // namespace mbgl
