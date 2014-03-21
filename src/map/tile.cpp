#include <llmr/map/tile.hpp>

#include <cassert>

using namespace llmr;

Tile::Tile(const ID& id)
    : id(id) {
}

Tile::ID Tile::ID::parent(int8_t parent_z) const {
    assert(parent_z < z);
    int32_t dim = pow(2, z - parent_z);
    return Tile::ID{
        static_cast<int8_t>(parent_z),
        static_cast<int32_t>(floor((float)x / dim)),
        static_cast<int32_t>(y / dim)
    };
}

std::forward_list<Tile::ID> Tile::ID::children(int32_t child_z) const {
    assert(child_z > z);
    int32_t factor = pow(2, child_z - z);

    std::forward_list<ID> children;
    for (int32_t ty = y * factor, y_max = (y + 1) * factor; ty < y_max; ++ty) {
        for (int32_t tx = x * factor, x_max = (x + 1) * factor; tx < x_max; ++tx) {
            children.emplace_front(child_z, tx, ty);
        }
    }
    return children;
}

void Tile::ID::normalize() {
    int32_t dim = pow(2, z);
    while (x < 0) x += dim;
    while (x >= dim) x -= dim;
}

Tile::ID Tile::ID::normalized() const {
    ID pos(z, x, y);
    pos.normalize();
    return pos;
}
