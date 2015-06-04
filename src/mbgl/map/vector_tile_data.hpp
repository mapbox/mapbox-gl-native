#ifndef MBGL_MAP_VECTOR_TILE_DATA
#define MBGL_MAP_VECTOR_TILE_DATA

#include <mbgl/map/tile_data.hpp>
#include <mbgl/map/tile_worker.hpp>

namespace mbgl {

class Bucket;
class SourceInfo;
class StyleLayer;
class Style;

class VectorTileData : public TileData {
public:
    VectorTileData(const TileID&,
                   Style&,
                   const SourceInfo&,
                   float angle_,
                   bool collisionDebug_);
    ~VectorTileData();

    void redoPlacement(float angle, bool collisionDebug) override;
    void setState(const State& state) override;

    Bucket* getBucket(const StyleLayer&) override;
    size_t countBuckets() const;

    void request(Worker&,
                 float pixelRatio,
                 const std::function<void()>& callback) override;

    bool reparse(Worker&, std::function<void ()> callback) override;

protected:
    void redoPlacement();

    Worker& worker;
    TileWorker workerData;

private:
    float lastAngle = 0;
    float currentAngle;
    bool lastCollisionDebug = 0;
    bool currentCollisionDebug = 0;
    bool redoingPlacement = false;
    void endRedoPlacement();
};

}

#endif
