#pragma once

#include <mbgl/tile/tile_id.hpp>
#include <mbgl/util/clip_id.hpp>

#include <unordered_set>
#include <vector>
#include <unordered_map>

namespace mbgl {
namespace algorithm {

class ClipIDGenerator {
private:
    struct Leaf {
        Leaf(ClipID&);
        void add(const CanonicalTileID &p);
        bool operator==(const Leaf &other) const;

        std::unordered_set<CanonicalTileID> children;
        ClipID& clip;
    };

    uint8_t bit_offset = 0;
    std::unordered_multimap<UnwrappedTileID, Leaf> pool;

public:
    template <typename Renderables>
    void update(Renderables& renderables);

    std::map<UnwrappedTileID, ClipID> getStencils() const;
};

} // namespace algorithm
} // namespace mbgl
