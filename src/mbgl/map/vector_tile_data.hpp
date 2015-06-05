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

    void redoPlacement(float angle, bool collisionDebug) override;

    Bucket* getBucket(const StyleLayer&) override;
    size_t countBuckets() const;

    void request(Worker&,
                 float pixelRatio,
                 const std::function<void()>& callback) override;

    bool reparse(Worker&, std::function<void ()> callback) override;

    void cancel() override;

protected:
    // We let subclasses override setState() so they
    // can intercept the state change and react accordingly.
    void setState(const State&);

    // Set the internal parsing state to true so we prevent
    // multiple workers to parse the same tile in parallel,
    // which can happen if the tile is in the "partial" state.
    // It will return true if is possible to start pasing the
    // tile or false if not (so some other worker is already
    // parsing the tile).
    bool mayStartParsing();

    void endParsing();

    // Error message to be set in case of request
    // and parsing errors.
    void setError(const std::string& message);

    void redoPlacement();

    const SourceInfo& source;

    Request *req = nullptr;
    std::string data;

    Worker& worker;
    TileWorker workerData;

    std::unique_ptr<WorkRequest> workRequest;
    std::atomic_flag parsing = ATOMIC_FLAG_INIT;

private:
    // Returns true if the TileData is in a final state and we cannot
    // make changes to it anymore.
    inline bool isImmutable() const {
        return state == State::parsed || state == State::obsolete;
    }

    float lastAngle = 0;
    float currentAngle;
    bool lastCollisionDebug = 0;
    bool currentCollisionDebug = 0;
    bool redoingPlacement = false;
    void endRedoPlacement();
};

}

#endif
