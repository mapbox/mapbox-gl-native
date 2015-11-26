#ifndef MBGL_UTIL_WORKER
#define MBGL_UTIL_WORKER

#include <mbgl/util/noncopyable.hpp>
#include <mbgl/util/thread.hpp>
#include <mbgl/map/tile_worker.hpp>

#include <functional>
#include <memory>

namespace mbgl {

class WorkRequest;
class RasterBucket;
class GeometryTileLoader;

using RasterTileParseResult = mapbox::util::variant<
    std::unique_ptr<Bucket>, // success
    std::string>;            // error

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

    using Request = std::unique_ptr<WorkRequest>;

    Request parseRasterTile(std::unique_ptr<RasterBucket> bucket,
                            std::shared_ptr<const std::string> data,
                            std::function<void(RasterTileParseResult)> callback);

    Request parseGeometryTile(TileWorker&,
                              std::vector<util::ptr<StyleLayer>>,
                              std::unique_ptr<GeometryTile>,
                              PlacementConfig,
                              std::function<void(TileParseResult)> callback);

    Request parsePendingGeometryTileLayers(TileWorker&,
                                           std::function<void(TileParseResult)> callback);

    Request redoPlacement(TileWorker&,
                          std::vector<util::ptr<StyleLayer>>,
                          const std::unordered_map<std::string, std::unique_ptr<Bucket>>&,
                          PlacementConfig config,
                          std::function<void()> callback);

private:
    class Impl;
    std::vector<std::unique_ptr<util::Thread<Impl>>> threads;
    std::size_t current = 0;
};
}

#endif
