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
    struct ID {
        int8_t z = 0;
        int32_t x = 0, y = 0;

        inline ID(int8_t z, int32_t x, int32_t y)
            : z(z), x(x), y(y) {
        }

        inline operator uint64_t() const {
            uint32_t dim = 1 << z;
            return ((dim * y + x) * 32) + z;
        }

        inline bool operator==(const ID& rhs) const {
            return z == rhs.z && x == rhs.x && y == rhs.y;
        }

        ID parent(int8_t z) const;
        void normalize();
        ID normalized() const;
        std::forward_list<ID> children(int32_t z) const;
    };

public:
    Tile(const ID& id);

public:
    const Tile::ID id;
    uint8_t clip_id;
    mat4 matrix;
    std::shared_ptr<TileData> data;

};

}

#endif
