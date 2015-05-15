#include <mbgl/util/clip_id.hpp>

#include <mbgl/platform/log.hpp>
#include <mbgl/util/math.hpp>
#include <mbgl/map/tile.hpp>

#include <list>
#include <vector>
#include <bitset>
#include <cassert>
#include <iostream>
#include <algorithm>

namespace mbgl {

ClipIDGenerator::Leaf::Leaf(Tile &tile_) : tile(tile_) {}

void ClipIDGenerator::Leaf::add(const TileID &p) {
    if (p.isChildOf(tile.id)) {
        // Ensure that no already present child is a parent of the new p.
        for (const auto& child : children) {
            if (p.isChildOf(child))
                return;
        }
        children.push_front(p);
    }
}

bool ClipIDGenerator::Leaf::operator==(const Leaf &other) const {
    return tile.id == other.tile.id && children == other.children;
}

bool ClipIDGenerator::reuseExisting(Leaf &leaf) {
    for (const auto& pool : pools) {
        auto existing = std::find(pool.begin(), pool.end(), leaf);
        if (existing != pool.end()) {
            leaf.tile.clip = existing->tile.clip;
            return true;
        }
    }
    return false;
}

void ClipIDGenerator::update(std::forward_list<Tile *> tiles) {
    Pool pool;

    tiles.sort([](const Tile *a, const Tile *b) {
        return a->id < b->id;
    });

    const auto end = tiles.end();
    for (auto it = tiles.begin(); it != end; it++) {
        if (!*it) {
            // Handle null pointers.
            continue;
        }

        Tile &tile = **it;
        Leaf clip { tile };

        // Try to add all remaining ids as children. We sorted the tile list
        // by z earlier, so all preceding items cannot be children of the current
        // tile.
        for (auto child_it = std::next(it); child_it != end; child_it++) {
            clip.add((*child_it)->id);
        }
        clip.children.sort();

        // Loop through all existing pools and try to find a matching ClipID.
        if (!reuseExisting(clip)) {
            // We haven't found an existing clip ID
            pool.push_back(std::move(clip));
        }
    }

    if (pool.size()) {
        const uint32_t bit_count = util::ceil_log2(pool.size() + 1);
        const std::bitset<8> mask = uint64_t(((1ul << bit_count) - 1) << bit_offset);

        // We are starting our count with 1 since we need at least 1 bit set to distinguish between
        // areas without any tiles whatsoever and the current area.
        uint8_t count = 1;
        for (auto& leaf : pool) {
            leaf.tile.clip.mask = mask;
            leaf.tile.clip.reference = uint32_t(count++) << bit_offset;
        }

        bit_offset += bit_count;
        pools.push_front(std::move(pool));
    }

    if (bit_offset > 8) {
        Log::Error(Event::OpenGL, "stencil mask overflow");
    }
}

}
