#ifndef LLMR_MAP_TILE
#define LLMR_MAP_TILE

#include "../geometry/debug_font_buffer.hpp"
#include "../geometry/linevertexbuffer.hpp"

#include <stdint.h>
#include <forward_list>
#include <mutex>
#include <llmr/util/vec.hpp>

namespace llmr {

typedef vec3<int32_t> tile_id;

class tile {
public:
    typedef std::shared_ptr<tile> ptr;
    enum state {
        initial,
        loading,
        ready,
        obsolete
    };

public:
    tile(tile_id id);
    ~tile();

    void setData(uint8_t *data, uint32_t bytes);
    bool parse();
    void parseLayer(const uint8_t *data, uint32_t bytes);
    void parseFeature(const uint8_t *data, uint32_t bytes);
    void loadGeometry(const uint8_t *data, uint32_t bytes);

    void cancel();

    const std::string toString() const;

    static tile_id parent(const tile_id& id, int32_t z);
    static std::forward_list<tile_id> children(const tile_id& id, int32_t z);

public:
    const tile_id id;
    state state;
    linevertexbuffer lineVertex;
    debug_font_buffer debugFontVertex;

private:
    // Source data
    uint8_t *data;
    uint32_t bytes;

};

}

#endif
