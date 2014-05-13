#ifndef LLMR_MAP_TILE
#define LLMR_MAP_TILE

#include <llmr/util/vec.hpp>
#include <llmr/util/mat4.hpp>
#include <llmr/util/noncopyable.hpp>

#include <cstdint>
#include <forward_list>
#include <string>
#include <bitset>

namespace llmr {

class TileData;

struct ClipID {
    explicit ClipID() {}
    explicit ClipID(const std::bitset<8> &mask, uint8_t length) : mask(mask), length(length) {}
    explicit ClipID(const std::string &mask, uint8_t length) : mask(mask), length(length) {}
    std::bitset<8> mask;
    uint8_t length = 0;
};

class Tile : private util::noncopyable {
public:
    struct ID {
        const int16_t w = 0;
        const int8_t z = 0;
        const int32_t x = 0, y = 0;

        inline explicit ID(int8_t z, int32_t x, int32_t y)
            : w((x < 0 ? x - (1 << z) + 1 : x) / (1 << z)), z(z), x(x), y(y) {}

        inline uint64_t to_uint64() const {
            return ((pow(2, z) * y + x) * 32) + z;
        }

        inline operator std::string() const {
            return std::to_string(z) + "/" + std::to_string(x) + "/" + std::to_string(y);
        }

        inline bool operator==(const ID& rhs) const {
            return w == rhs.w && z == rhs.z && x == rhs.x && y == rhs.y;
        }

        inline bool operator<(const ID &rhs) const {
            if (w != rhs.w) return w < rhs.w;
            if (z != rhs.z) return z < rhs.z;
            if (x != rhs.x) return x < rhs.x;
            return y < rhs.y;
        }

        ID parent(int8_t z) const;
        ID normalized() const;
        std::forward_list<ID> children(int32_t z) const;
        bool isChildOf(const Tile::ID &id) const;
    };

public:
    explicit Tile(const ID& id);

public:
    const Tile::ID id;
    ClipID clip;
    mat4 matrix;
    std::shared_ptr<TileData> data;
};

}

#endif
