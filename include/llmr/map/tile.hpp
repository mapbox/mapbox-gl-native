#ifndef LLMR_MAP_TILE
#define LLMR_MAP_TILE

#include <llmr/util/vec.hpp>
#include <llmr/util/mat4.hpp>
#include <llmr/util/noncopyable.hpp>

#include <cstdint>
#include <forward_list>

namespace llmr {

class TileData;

class Tile : private util::noncopyable {
public:
    typedef vec3<int32_t> ID;

public:
    Tile(const ID& id);

    static ID parent(const ID& id, int32_t z);
    static ID normalize(const ID& id);
    static std::forward_list<ID> children(const ID& id, int32_t z);


public:
    const Tile::ID id;
    uint8_t clip_id;
    mat4 matrix;
    std::shared_ptr<TileData> data;

};

}

#endif
