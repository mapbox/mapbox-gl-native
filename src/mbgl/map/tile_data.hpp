#ifndef MBGL_MAP_TILE_DATA
#define MBGL_MAP_TILE_DATA

#include <mbgl/map/tile_id.hpp>
#include <mbgl/renderer/debug_bucket.hpp>
#include <mbgl/geometry/debug_font_buffer.hpp>

#include <mbgl/util/noncopyable.hpp>
#include <mbgl/util/ptr.hpp>
#include <mbgl/util/work_request.hpp>

#include <atomic>
#include <string>
#include <functional>

namespace mbgl {

class Environment;
class Painter;
class SourceInfo;
class StyleLayer;
class Request;
class Worker;

class TileData : private util::noncopyable {
public:
    enum class State {
        invalid,
        initial,
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

    void request(Worker&, float pixelRatio, std::function<void ()> callback);
    void reparse(Worker&, std::function<void ()> callback);
    void cancel();
    const std::string toString() const;

    inline bool ready() const {
        return isReadyState(state);
    }

    void endParsing();

    // Override this in the child class.
    virtual void parse() = 0;
    virtual Bucket* getBucket(StyleLayer const &layer_desc) = 0;

    const TileID id;
    const std::string name;
    std::atomic<State> state;
    std::atomic_flag parsing;

protected:
    // Set the internal parsing state to true so we prevent
    // multiple workers to parse the same tile in parallel,
    // which can happen if the tile is in the "partial" state.
    // It will return true if is possible to start pasing the
    // tile or false if not (so some other worker is already
    // parsing the tile).
    bool mayStartParsing();

    const SourceInfo& source;
    Environment& env;

    Request *req = nullptr;
    std::string data;

    WorkRequest workRequest;

    // Contains the tile ID string for painting debug information.
    DebugFontBuffer debugFontBuffer;

public:
    DebugBucket debugBucket;
};

}

#endif
