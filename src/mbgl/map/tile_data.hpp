#ifndef MBGL_MAP_TILE_DATA
#define MBGL_MAP_TILE_DATA

#include <mbgl/map/tile_id.hpp>
#include <mbgl/renderer/debug_bucket.hpp>
#include <mbgl/geometry/debug_font_buffer.hpp>

#include <mbgl/util/noncopyable.hpp>
#include <mbgl/util/ptr.hpp>

#include <atomic>
#include <string>
#include <functional>

namespace mbgl {

class Painter;
class SourceInfo;
class StyleLayer;
class Request;
class Worker;
class WorkRequest;
class TransformState;

class TileData : private util::noncopyable {
public:
    // initial:
    //   Initial state, only used when the TileData object is created.
    //
    // invalid:
    //   FIXME: This state has a bit of overlap with 'initial' and 'obsolete'.
    //
    //   We report TileData being 'invalid' on Source::hasTile if we don't have the
    //   TileData yet, then Source creates a request. This is misleading because
    //   the TileData object is not effectively on the 'invalid' state and will
    //   cause tiles on 'invalid' state to get reloaded.
    //
    //   Raster tiles (only) are changing to 'invalid' on error, which will cause a
    //   re-request of the tile.
    //
    // loading:
    //   A request to the FileSource was made for the actual tile data and TileData
    //   is waiting for it to arrive.
    //
    // loaded:
    //   The actual tile data has arrived and the tile can be parsed.
    //
    // partial:
    //   TileData is partially parsed, some buckets are still waiting for dependencies
    //   to arrive, but it is good for rendering. Partial tiles can also be re-parsed,
    //   but might remain in the same state if dependencies are still missing.
    //
    // parsed:
    //   TileData is fully parsed and its contents won't change from this point. This
    //   is the only state which is safe to cache this object.
    //
    // obsolete:
    //   The TileData can go to obsolete from any state, due to parsing or loading error,
    //   request cancellation or because the tile is no longer in use.
    enum class State {
        initial,
        invalid,
        loading,
        loaded,
        partial,
        parsed,
        obsolete
    };

    // Tile data considered "Ready" can be used for rendering. Data in
    // partial state is still waiting for network resources but can also
    // be rendered, although layers will be missing.
    inline static bool isReadyState(const State& state) {
        return state == State::partial || state == State::parsed;
    }

    TileData(const TileID&, const SourceInfo&);
    ~TileData();

    void request(Worker&, float pixelRatio, const std::function<void()>& callback);

    // Schedule a tile reparse on a worker thread and call the callback on
    // completion. It will return true if the work was schedule or false it was
    // not, which can occur if the tile is already being parsed by another
    // worker (see "mayStartParsing()").
    bool reparse(Worker&, std::function<void ()> callback);

    void cancel();
    const std::string toString() const;

    inline bool isReady() const {
        return isReadyState(state);
    }

    // Returns true if the TileData is in a final state and we cannot
    // make changes to it anymore.
    inline bool isImmutable() const {
        return state == State::parsed || state == State::obsolete;
    }

    // We let subclasses override setState() so they
    // can intercept the state change and react accordingly.
    virtual void setState(const State& state);
    inline State getState() const {
        return state;
    }

    void endParsing();

    // Error message to be set in case of request
    // and parsing errors.
    void setError(const std::string& message);

    std::string getError() const {
        return error;
    }

    // Override this in the child class.
    virtual void parse() = 0;
    virtual Bucket* getBucket(StyleLayer const &layer_desc) = 0;

    virtual void redoPlacement(float, bool) {}

    const TileID id;
    const std::string name;
    std::atomic_flag parsing = ATOMIC_FLAG_INIT;

protected:
    // Set the internal parsing state to true so we prevent
    // multiple workers to parse the same tile in parallel,
    // which can happen if the tile is in the "partial" state.
    // It will return true if is possible to start pasing the
    // tile or false if not (so some other worker is already
    // parsing the tile).
    bool mayStartParsing();

    const SourceInfo& source;

    Request *req = nullptr;
    std::string data;

    std::unique_ptr<WorkRequest> workRequest;

private:
    std::atomic<State> state;

    std::string error;

protected:
    // Contains the tile ID string for painting debug information.
    DebugFontBuffer debugFontBuffer;

public:
    DebugBucket debugBucket;
};

}

#endif
