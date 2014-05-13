#ifndef LLMR_MAP_TILE_DATA
#define LLMR_MAP_TILE_DATA

#include <llmr/map/tile.hpp>
#include <llmr/util/noncopyable.hpp>
#include <llmr/platform/platform.hpp>
#include <llmr/geometry/vao.hpp>
#include <llmr/renderer/debug_bucket.hpp>

#include <cstdint>
#include <string>
#include <memory>
#include <atomic>

namespace llmr {

class Map;
class Painter;
class LayerDescription;

class TileData : public std::enable_shared_from_this<TileData>,
             private util::noncopyable {
public:
    struct exception : std::exception {};
    struct geometry_too_long_exception : exception {};

public:
    typedef std::shared_ptr<TileData> Ptr;

    enum class State {
        invalid,
        initial,
        loading,
        loaded,
        parsed,
        obsolete
    };

public:
    TileData(Tile::ID id, Map &map, const std::string url);
    ~TileData();

    void request();
    void cancel();
    void reparse();
    const std::string toString() const;

    // Override this in the child class.
    virtual void parse() = 0;
    virtual void render(Painter &painter, const LayerDescription& layer_desc) = 0;
    virtual bool hasData(const LayerDescription& layer_desc) const = 0;


public:
    const Tile::ID id;
    std::atomic<State> state;

protected:
    Map &map;

    // Request-related information.
    const std::string url;
    std::weak_ptr<platform::Request> req;
    std::string data;

    // Contains the tile ID string for painting debug information.
    DebugFontBuffer debugFontBuffer;

public:
    DebugBucket debugBucket;
};

}

#endif
