#ifndef MBGL_MAP_TILE_ID
#define MBGL_MAP_TILE_ID

#include <cstdint>
#include <cmath>
#include <string>
#include <functional>
#include <forward_list>

namespace mbgl {

class TileID {
public:
    const int16_t w = 0;
    const int8_t z = 0;
    const int32_t x = 0, y = 0;

    inline explicit TileID(int8_t z_, int32_t x_, int32_t y_)
        : w((x_ < 0 ? x_ - (1 << z_) + 1 : x_) / (1 << z_)), z(z_), x(x_), y(y_) {}

    inline uint64_t to_uint64() const {
        return ((std::pow(2, z) * y + x) * 32) + z;
    }

    struct Hash {
        std::size_t operator()(const TileID& id) const {
            return std::hash<uint64_t>()(id.to_uint64());
        }
    };

    inline bool operator==(const TileID& rhs) const {
        return w == rhs.w && z == rhs.z && x == rhs.x && y == rhs.y;
    }

    inline bool operator!=(const TileID& rhs) const {
        return !operator==(rhs);
    }

    inline bool operator<(const TileID& rhs) const {
        if (w != rhs.w) return w < rhs.w;
        if (z != rhs.z) return z < rhs.z;
        if (x != rhs.x) return x < rhs.x;
        return y < rhs.y;
    }

    TileID parent(int8_t z) const;
    TileID normalized() const;
    std::forward_list<TileID> children(int32_t z) const;
    bool isChildOf(const TileID&) const;
    operator std::string() const;
};

}

#endif
