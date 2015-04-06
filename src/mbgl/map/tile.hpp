#ifndef MBGL_MAP_TILE
#define MBGL_MAP_TILE

#include <mbgl/util/mat4.hpp>
#include <mbgl/util/noncopyable.hpp>
#include <mbgl/util/ptr.hpp>
#include <mbgl/map/tile_id.hpp>

#include <bitset>
#include <string>

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
    static std::forward_list<TileID> cover(int8_t z, const box& bounds);

    explicit Tile(const TileID&);

    const TileID id;
    ClipID clip;
    mat4 matrix;
    util::ptr<TileData> data;
};

}

#endif
