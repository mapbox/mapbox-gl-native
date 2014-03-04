#include <llmr/map/tile.hpp>

#include <cassert>

using namespace llmr;

Tile::ID Tile::parent(const ID& id, int32_t z) {
    assert(z < id.z);
    ID pos(id);
    while (pos.z > z) {
        --pos.z;
        pos.x = floor(pos.x / 2);
        pos.y = floor(pos.y / 2);
    }
    return pos;
}


std::forward_list<Tile::ID> Tile::children(const ID& id, int32_t z) {
    assert(z > id.z);
    int32_t factor = pow(2, z - id.z);

    std::forward_list<ID> children;
    for (int32_t y = id.y * factor, y_max = (id.y + 1) * factor; y < y_max; ++y) {
        for (int32_t x = id.x * factor, x_max = (id.x + 1) * factor; x < x_max; ++x) {
            children.emplace_front(x, y, z);
        }
    }
    return children;
}

