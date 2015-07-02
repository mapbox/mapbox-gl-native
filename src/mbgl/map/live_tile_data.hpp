#ifndef MBGL_MAP_LIVE_TILE_DATA
#define MBGL_MAP_LIVE_TILE_DATA

#include <mbgl/map/tile_data.hpp>
#include <mbgl/map/tile_worker.hpp>

namespace mbgl {

class Style;
class SourceInfo;
class WorkRequest;
class LiveTile;

class LiveTileData : public TileData {
public:
    LiveTileData(const TileID&,
                 const LiveTile*,
                 Style&,
                 const SourceInfo&,
                 std::function<void ()> callback);
    ~LiveTileData();

    void cancel() override;
    Bucket* getBucket(const StyleLayer&) override;

private:
    Worker& worker;
    TileWorker tileWorker;
    std::unique_ptr<WorkRequest> workRequest;
};

}

#endif
