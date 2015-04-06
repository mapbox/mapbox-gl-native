#include <mbgl/map/tile_id.hpp>
#include <mbgl/util/string.hpp>

#include <cassert>

namespace mbgl {

TileID TileID::parent(int8_t parent_z) const {
    assert(parent_z < z);
    int32_t dim = std::pow(2, z - parent_z);
    return TileID{
        parent_z,
        (x >= 0 ? x : x - dim + 1) / dim,
        y / dim
    };
}

std::forward_list<TileID> TileID::children(int32_t child_z) const {
    assert(child_z > z);
    int32_t factor = std::pow(2, child_z - z);

    std::forward_list<TileID> child_ids;
    for (int32_t ty = y * factor, y_max = (y + 1) * factor; ty < y_max; ++ty) {
        for (int32_t tx = x * factor, x_max = (x + 1) * factor; tx < x_max; ++tx) {
            child_ids.emplace_front(child_z, tx, ty);
        }
    }
    return child_ids;
}

TileID TileID::normalized() const {
    int32_t dim = std::pow(2, z);
    int32_t nx = x, ny = y;
    while (nx < 0) nx += dim;
    while (nx >= dim) nx -= dim;
    return TileID { z, nx, ny };
}

bool TileID::isChildOf(const TileID &parent_id) const {
    if (parent_id.z >= z || parent_id.w != w) {
        return false;
    }
    int32_t scale = std::pow(2, z - parent_id.z);
    return parent_id.x == ((x < 0 ? x - scale + 1 : x) / scale) &&
           parent_id.y == y / scale;
}

TileID::operator std::string() const {
    return util::toString(z) + "/" + util::toString(x) + "/" + util::toString(y);
}

}
