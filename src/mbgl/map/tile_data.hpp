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

class Environment;
class Painter;
class SourceInfo;
class StyleLayer;
class Request;
class Worker;

class TileData : public std::enable_shared_from_this<TileData>,
             private util::noncopyable {
public:
    enum class State {
        invalid,
        initial,
        loading,
        loaded,
        parsed,
        obsolete
    };

    TileData(const TileID&, const SourceInfo&);
    ~TileData();

    void request(Worker&, float pixelRatio, std::function<void ()> callback);
    void reparse(Worker&, std::function<void ()> callback);
    void cancel();
    const std::string toString() const;

    inline bool ready() const {
        return state == State::parsed;
    }

    // Override this in the child class.
    virtual void parse() = 0;
    virtual void render(Painter &painter, const StyleLayer &layer_desc, const mat4 &matrix) = 0;
    virtual bool hasData(StyleLayer const &layer_desc) const = 0;

    const TileID id;
    const std::string name;
    std::atomic<State> state;

protected:
    const SourceInfo& source;
    Environment& env;

    Request *req = nullptr;
    std::string data;

    // Contains the tile ID string for painting debug information.
    DebugFontBuffer debugFontBuffer;

public:
    DebugBucket debugBucket;
};

}

#endif
