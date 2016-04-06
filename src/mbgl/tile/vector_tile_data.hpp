#ifndef MBGL_MAP_VECTOR_TILE_DATA
#define MBGL_MAP_VECTOR_TILE_DATA

#include <mbgl/tile/tile_data.hpp>
#include <mbgl/tile/tile_worker.hpp>
#include <mbgl/text/placement_config.hpp>
#include <mbgl/util/id.hpp>

#include <atomic>
#include <memory>
#include <map>

namespace mbgl {

class Style;
class AsyncRequest;
class GeometryTileMonitor;
class Source;

class VectorTileData : public TileData {
public:
    VectorTileData(const TileID&,
                   std::unique_ptr<GeometryTileMonitor> monitor,
                   util::ID<Source> sourceID,
                   Style&,
                   const MapMode,
                   const std::function<void(std::exception_ptr)>& callback);

    ~VectorTileData();

    Bucket* getBucket(const StyleLayer&) override;

    bool parsePending(std::function<void(std::exception_ptr)> callback) override;

    void redoPlacement(PlacementConfig config, const std::function<void()>&) override;
    void redoPlacement(const std::function<void()>&) override;

    void cancel() override;

private:
    Style& style;
    Worker& worker;
    TileWorker tileWorker;

    std::unique_ptr<GeometryTileMonitor> monitor;
    std::unique_ptr<AsyncRequest> tileRequest;
    std::unique_ptr<AsyncRequest> workRequest;

    // Contains all the Bucket objects for the tile. Buckets are render
    // objects and they get added by tile parsing operations.
    std::map<util::ID<StyleLayer>, std::unique_ptr<Bucket>> buckets;

    // Stores the placement configuration of the text that is currently placed on the screen.
    PlacementConfig placedConfig;

    // Stores the placement configuration of how the text should be placed. This isn't necessarily
    // the one that is being displayed.
    PlacementConfig targetConfig;
};

} // namespace mbgl

#endif
