#ifndef MBGL_MAP_TILE_DATA
#define MBGL_MAP_TILE_DATA

#include <mbgl/map/tile.hpp>
#include <mbgl/renderer/debug_bucket.hpp>
#include <mbgl/geometry/debug_font_buffer.hpp>

#include <mbgl/util/noncopyable.hpp>

#include <atomic>
#include <exception>
#include <iosfwd>
#include <memory>
#include <string>

namespace mbgl {

class Map;
class Painter;
class SourceInfo;
class StyleLayer;

namespace platform { class Request; }

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
    TileData(Tile::ID id, Map &map, const SourceInfo &source);
    ~TileData();

    void request();
    void cancel();
    void reparse();
    const std::string toString() const;

    // Override this in the child class.
    virtual void beforeParse();
    virtual void parse() = 0;
    virtual void afterParse();
    virtual void render(Painter &painter, std::shared_ptr<StyleLayer> layer_desc, const mat4 &matrix) = 0;
    virtual bool hasData(std::shared_ptr<StyleLayer> layer_desc) const = 0;


public:
    const Tile::ID id;
    std::atomic<State> state;

protected:
    Map &map;

public:
    const SourceInfo &source;

    // Request-related information.
    const std::string url;

protected:
    std::weak_ptr<platform::Request> req;
    std::string data;

    // Contains the tile ID string for painting debug information.
    DebugFontBuffer debugFontBuffer;

public:
    DebugBucket debugBucket;
};

}

#endif
