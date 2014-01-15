#ifndef LLMR_MAP_TILE
#define LLMR_MAP_TILE

#include "../geometry/linevertexbuffer.hpp"

#include <stdint.h>
#include <forward_list>
#include <mutex>
#include <llmr/util/vec2.hpp>

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

    // static int64_t toID(int32_t z, int32_t x, int32_t y, int32_t w = 0);
    // static vec4<int32_t> fromID(int64_t id);

public:
    const tile_id id;
    state state;
    linevertexbuffer lineVertex;

private:
    // Source data
    uint8_t *data;
    uint32_t bytes;

};

}

#endif
