#ifndef LLMR_MAP_TILE
#define LLMR_MAP_TILE

#include <llmr/util/vec.hpp>

#include <cstdint>
#include <forward_list>

namespace llmr {

class Tile {
public:
    typedef vec3<int32_t> ID;

public:
    static ID parent(const ID& id, int32_t z);
    static std::forward_list<ID> children(const ID& id, int32_t z);
};

}

#endif
