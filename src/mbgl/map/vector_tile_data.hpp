#ifndef MBGL_MAP_VECTOR_TILE_DATA
#define MBGL_MAP_VECTOR_TILE_DATA

#include <mbgl/map/tile_data.hpp>
#include <mbgl/map/tile_worker.hpp>

#include <atomic>

namespace mbgl {

class SourceInfo;
class Request;
class Bucket;
class SourceInfo;
class StyleLayer;
class Style;
class WorkRequest;

class VectorTileData : public TileData {
public:
    VectorTileData(const TileID&,
                   Style&,
                   const SourceInfo&,
                   float angle_,
                   bool collisionDebug_);
    ~VectorTileData();

    Bucket* getBucket(const StyleLayer&) override;
    size_t countBuckets() const;

    void request(float pixelRatio,
                 const std::function<void()>& callback) override;

    bool reparse(std::function<void ()> callback) override;

    void redoPlacement(float angle, bool collisionDebug) override;

    void cancel() override;

protected:
    Worker& worker;
    TileWorker tileWorker;
    std::unique_ptr<WorkRequest> workRequest;
    bool parsing = false;

private:
    const SourceInfo& source;
    Request* req = nullptr;
    std::string data;

    float lastAngle = 0;
    float currentAngle;
    bool lastCollisionDebug = 0;
    bool currentCollisionDebug = 0;
    bool redoingPlacement = false;
};

}

#endif
