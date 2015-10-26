#ifndef MBGL_MAP_LIVE_TILE_DATA
#define MBGL_MAP_LIVE_TILE_DATA

#include <mbgl/map/tile_data.hpp>
#include <mbgl/map/tile_worker.hpp>

namespace mbgl {

class Style;
class SourceInfo;
class WorkRequest;
class AnnotationTile;

class LiveTileData : public TileData {
public:
    LiveTileData(const TileID&,
                 std::unique_ptr<AnnotationTile>,
                 Style&,
                 const SourceInfo&,
                 std::function<void ()> callback);
    ~LiveTileData();

    bool parsePending(std::function<void ()> callback) override;

    void redoPlacement(PlacementConfig config) override;
    void redoPlacement();

    void cancel() override;
    Bucket* getBucket(const StyleLayer&) override;

private:
    Worker& worker;
    TileWorker tileWorker;
    std::unique_ptr<WorkRequest> workRequest;
    std::unique_ptr<AnnotationTile> tile;

    // Contains all the Bucket objects for the tile. Buckets are render
    // objects and they get added by tile parsing operations.
    std::unordered_map<std::string, std::unique_ptr<Bucket>> buckets;

    // Stores the placement configuration of the text that is currently placed on the screen.
    PlacementConfig placedConfig;

    // Stores the placement configuration of how the text should be placed. This isn't necessarily
    // the one that is being displayed.
    PlacementConfig targetConfig;
};

}

#endif
