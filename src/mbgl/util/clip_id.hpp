#ifndef MBGL_UTIL_CLIP_IDS
#define MBGL_UTIL_CLIP_IDS

#include <mbgl/map/tile_id.hpp>

#include <bitset>
#include <string>
#include <list>
#include <set>
#include <vector>
#include <forward_list>
#include <map>

namespace mbgl {

class Tile;
class TileID;

struct ClipID {
    inline ClipID() {}
    inline ClipID(const std::string &mask_, const std::string &reference_) : mask(mask_), reference(reference_) {}

    std::bitset<8> mask;
    std::bitset<8> reference;

    inline bool operator==(const ClipID &other) const {
        return mask == other.mask && reference == other.reference;
    }
};

class ClipIDGenerator {
private:
    struct Leaf {
        Leaf(Tile &tile);
        void add(const TileID &p);
        bool operator==(const Leaf &other) const;

        Tile &tile;
        std::forward_list<TileID> children;
    };

    typedef std::vector<Leaf> Pool;
    std::forward_list<Pool> pools;

    const size_t kStencilBufferBitLength = 8;
    // We are starting our count with 1 since we need at least 1 bit set to distinguish between
    // areas without any tiles whatsoever and the current area.
    uint32_t clipIdSerial = 1;

private:
    bool reuseExisting(Leaf &leaf);

public:
    void update(std::forward_list<Tile *> tiles);
};


} // namespace mbgl

#endif
