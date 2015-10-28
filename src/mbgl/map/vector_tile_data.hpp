#ifndef MBGL_MAP_VECTOR_TILE_DATA
#define MBGL_MAP_VECTOR_TILE_DATA

#include <mbgl/map/tile_data.hpp>
#include <mbgl/map/tile_worker.hpp>
#include <mbgl/storage/request_holder.hpp>
#include <mbgl/text/placement_config.hpp>

#include <atomic>
#include <memory>
#include <unordered_map>

namespace mbgl {

class Style;
class WorkRequest;
class GeometryTileMonitor;

class VectorTileData : public TileData {
public:
    VectorTileData(const TileID&,
                   std::unique_ptr<GeometryTileMonitor> monitor,
                   std::string sourceID,
                   Style&,
                   const std::function<void()>& callback);

    ~VectorTileData();

    Bucket* getBucket(const StyleLayer&) override;

    bool parsePending(std::function<void()> callback) override;

    void redoPlacement(PlacementConfig config) override;
    void redoPlacement();

    void cancel() override;

private:
    Worker& worker;
    TileWorker tileWorker;

    std::unique_ptr<GeometryTileMonitor> monitor;
    std::unique_ptr<WorkRequest> workRequest;
    RequestHolder req;

    // Contains all the Bucket objects for the tile. Buckets are render
    // objects and they get added by tile parsing operations.
    std::unordered_map<std::string, std::unique_ptr<Bucket>> buckets;

    // Stores the placement configuration of the text that is currently placed on the screen.
    PlacementConfig placedConfig;

    // Stores the placement configuration of how the text should be placed. This isn't necessarily
    // the one that is being displayed.
    PlacementConfig targetConfig;
};

} // namespace mbgl

#endif
