#include <mbgl/map/tile.hpp>

#include <cassert>

using namespace mbgl;

#include <iostream>

Tile::Tile(const ID& id)
    : id(id) {
}

Tile::ID Tile::ID::parent(int8_t parent_z) const {
    assert(parent_z < z);
    int32_t dim = std::pow(2, z - parent_z);
    return Tile::ID{
        parent_z,
        (x >= 0 ? x : x - dim + 1) / dim,
        y / dim
    };
}

std::forward_list<Tile::ID> Tile::ID::children(int32_t child_z) const {
    assert(child_z > z);
    int32_t factor = std::pow(2, child_z - z);

    std::forward_list<ID> children;
    for (int32_t ty = y * factor, y_max = (y + 1) * factor; ty < y_max; ++ty) {
        for (int32_t tx = x * factor, x_max = (x + 1) * factor; tx < x_max; ++tx) {
            children.emplace_front(child_z, tx, ty);
        }
    }
    return children;
}

Tile::ID Tile::ID::normalized() const {
    int32_t dim = std::pow(2, z);
    int32_t nx = x, ny = y;
    while (nx < 0) nx += dim;
    while (nx >= dim) nx -= dim;
    return ID { z, nx, ny };
}

bool Tile::ID::isChildOf(const Tile::ID &parent) const {
    if (parent.z >= z || parent.w != w) {
        return false;
    }
    int32_t scale = std::pow(2, z - parent.z);
    return parent.x == ((x < 0 ? x - scale + 1 : x) / scale) &&
           parent.y == y / scale;
}
