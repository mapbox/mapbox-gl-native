#ifndef MBGL_MAP_TILE
#define MBGL_MAP_TILE

#include <mbgl/util/mat4.hpp>
#include <mbgl/util/noncopyable.hpp>
#include <mbgl/util/ptr.hpp>

#include <cstdint>
#include <bitset>
#include <cmath>
#include <cstdint>
#include <forward_list>
#include <iosfwd>
#include <string>
#include <functional>

namespace mbgl {

class TileData;
struct box;

struct ClipID {
    inline ClipID() {}
    inline ClipID(const std::string &mask_, const std::string &reference_) : mask(mask_), reference(reference_) {}

    std::bitset<8> mask;
    std::bitset<8> reference;

    inline bool operator==(const ClipID &other) const {
        return mask == other.mask && reference == other.reference;
    }
};

class Tile : private util::noncopyable {
public:
    struct ID {
        const int16_t w = 0;
        const int8_t z = 0;
        const int32_t x = 0, y = 0;

        inline explicit ID(int8_t z_, int32_t x_, int32_t y_)
            : w((x_ < 0 ? x_ - (1 << z_) + 1 : x_) / (1 << z_)), z(z_), x(x_), y(y_) {}

        inline uint64_t to_uint64() const {
            return ((std::pow(2, z) * y + x) * 32) + z;
        }

        struct Hash {
            std::size_t operator()(ID const& i) const {
                return std::hash<uint64_t>()(i.to_uint64());
            }
        };

        inline bool operator==(const ID& rhs) const {
            return w == rhs.w && z == rhs.z && x == rhs.x && y == rhs.y;
        }

        inline bool operator!=(const ID& rhs) const {
            return !operator==(rhs);
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
        operator std::string() const;
    };

    static std::forward_list<Tile::ID> cover(int8_t z, const box& bounds);

public:
    explicit Tile(const ID& id);

public:
    const Tile::ID id;
    ClipID clip;
    mat4 matrix;
    util::ptr<TileData> data;
};

}

#endif
