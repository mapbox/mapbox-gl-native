#ifndef MBGL_MAP_TILE_DATA
#define MBGL_MAP_TILE_DATA

#include <mbgl/util/noncopyable.hpp>
#include <mbgl/map/tile_id.hpp>
#include <mbgl/renderer/bucket.hpp>
#include <mbgl/text/placement_config.hpp>

#include <atomic>
#include <string>
#include <memory>
#include <functional>

namespace mbgl {

class StyleLayer;
class Worker;
class DebugBucket;

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

    static const char* StateToString(State);

    // Tile data considered "Ready" can be used for rendering. Data in
    // partial state is still waiting for network resources but can also
    // be rendered, although layers will be missing.
    inline static bool isReadyState(const State& state) {
        return state == State::partial || state == State::parsed;
    }

    TileData(const TileID&);
    virtual ~TileData();

    // Mark this tile as no longer needed and cancel any pending work.
    virtual void cancel() = 0;

    virtual Bucket* getBucket(const StyleLayer&) = 0;

    virtual bool parsePending(std::function<void ()>) { return true; }
    virtual void redoPlacement(PlacementConfig) {}

    bool isReady() const {
        return isReadyState(state);
    }

    State getState() const {
        return state;
    }

    std::string getError() const {
        return error;
    }

    const TileID id;

    // Contains the tile ID string for painting debug information.
    std::unique_ptr<DebugBucket> debugBucket;

protected:
    std::atomic<State> state;
    std::string error;
};

} // namespace mbgl

#endif
