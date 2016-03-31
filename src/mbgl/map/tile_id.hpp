#ifndef MBGL_MAP_TILE_ID
#define MBGL_MAP_TILE_ID

#include <cstdint>
#include <cmath>
#include <string>
#include <functional>
#include <forward_list>
#include <limits>

namespace mbgl {

class TileID {
public:
    const int16_t w = 0;
    const uint8_t z;
    const int32_t x;
    const int32_t y;
    const uint8_t sourceZ;

    explicit TileID(uint8_t z_, int32_t x_, int32_t y_, uint8_t sourceZ_)
        : w((x_ < 0 ? x_ - (1 << z_) + 1 : x_) / (1 << z_)), z(z_), x(x_), y(y_),
        sourceZ(sourceZ_) {}

    uint64_t to_uint64() const {
        return ((std::pow(2, z) * y + x) * 32) + z;
    }

    uint32_t overscaleFactor() const {
        return static_cast<uint32_t>(1) << (z - sourceZ);
    }

    bool operator==(const TileID& rhs) const {
        return w == rhs.w && z == rhs.z && x == rhs.x && y == rhs.y;
    }

    bool operator!=(const TileID& rhs) const {
        return !operator==(rhs);
    }

    bool operator<(const TileID& rhs) const {
        if (w != rhs.w) return w < rhs.w;
        if (z != rhs.z) return z < rhs.z;
        if (x != rhs.x) return x < rhs.x;
        return y < rhs.y;
    }

    float pixelsToTileUnits(float pixelValue, float zoom) const;
    TileID parent(uint8_t z, uint8_t sourceMaxZoom) const;
    TileID normalized() const;
    std::forward_list<TileID>
    children(uint8_t sourceMaxZoom = std::numeric_limits<uint8_t>::max()) const;
    bool isChildOf(const TileID&) const;
    operator std::string() const;
};

} // namespace mbgl

namespace std {
template <>
struct hash<mbgl::TileID> {
    typedef mbgl::TileID argument_type;
    typedef std::size_t result_type;

    result_type operator()(const mbgl::TileID& id) const {
            return std::hash<uint64_t>()(id.to_uint64());
    }
};
} // namespace std

#endif
