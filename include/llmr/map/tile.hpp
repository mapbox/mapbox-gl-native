#ifndef LLMR_MAP_TILE
#define LLMR_MAP_TILE

#include "../geometry/debug_font_buffer.hpp"
#include "../geometry/linevertexbuffer.hpp"
#include "../geometry/fill_buffer.hpp"

#include "layer.hpp"

#include <stdint.h>
#include <forward_list>
#include <mutex>
#include <llmr/util/vec.hpp>
#include <string>

namespace llmr {

struct pbf;

class Tile {
public:
    struct exception : std::exception {};
    struct geometry_too_long_exception : exception {};

public:
    typedef std::shared_ptr<Tile> Ptr;

    typedef vec3<int32_t> ID;

    enum state {
        initial,
        loading,
        ready,
        obsolete
    };

public:
    Tile(ID id);
    ~Tile();

    // Make noncopyable
    Tile(const Tile&) = delete;
    Tile(const Tile&&) = delete;
    Tile& operator=(const Tile&) = delete;
    Tile& operator=(const Tile && ) = delete;

    // Other functions
    void setData(uint8_t *data, uint32_t bytes);
    bool parse();
    void parseLayer(const pbf layer);

    std::shared_ptr<Bucket> createFillBucket(const pbf data);

    void cancel();

    const std::string toString() const;

    static ID parent(const ID& id, int32_t z);
    static std::forward_list<ID> children(const ID& id, int32_t z);

public:
    const ID id;
    state state;
    linevertexbuffer lineVertex;
    debug_font_buffer debugFontVertex;

    FillBuffer fillBuffer;

    std::forward_list<Layer> layers;

private:
    // Source data
    uint8_t *data;
    uint32_t bytes;

    std::mutex mtx;
};

}

#endif
