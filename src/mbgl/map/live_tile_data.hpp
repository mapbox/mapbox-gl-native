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

    bool reparse(std::function<void ()> callback) override;

    void cancel() override;
    Bucket* getBucket(const StyleLayer&) override;

private:
    Worker& worker;
    TileWorker tileWorker;
    std::unique_ptr<WorkRequest> workRequest;
    bool parsing = false;
    std::unique_ptr<AnnotationTile> tile;
};

}

#endif
