#include <mbgl/util/clip_id.hpp>

#include <mbgl/platform/log.hpp>
#include <mbgl/util/math.hpp>
#include <mbgl/util/std.hpp>
#include <mbgl/tile/tile.hpp>

#include <list>
#include <vector>
#include <bitset>
#include <cassert>
#include <iostream>
#include <algorithm>
#include <iterator>

namespace mbgl {

ClipIDGenerator::Leaf::Leaf(TileID id_, ClipID& clip_) : id(id_), clip(clip_) {}

void ClipIDGenerator::Leaf::add(const TileID &p) {
    if (p.isChildOf(id)) {
        // Ensure that no already present child is a parent of the new p.
        for (const auto& child : children) {
            if (p.isChildOf(child))
                return;
        }
        children.push_front(p);
    }
}

bool ClipIDGenerator::Leaf::operator==(const Leaf &other) const {
    return id == other.id && children == other.children;
}

void ClipIDGenerator::update(std::forward_list<Tile *> tiles) {
    tiles.sort([](const Tile *a, const Tile *b) {
        return a->id < b->id;
    });

    std::size_t size = 0;

    const auto end = tiles.end();
    for (auto it = tiles.begin(); it != end; it++) {
        if (!*it) {
            // Handle null pointers.
            continue;
        }

        Tile &tile = **it;
        // Use the actual zoom level for computing the clipping mask.
        Leaf leaf{ TileID{ tile.id.sourceZ, tile.id.x, tile.id.y, tile.id.sourceZ }, tile.clip };

        // Try to add all remaining ids as children. We sorted the tile list
        // by z earlier, so all preceding items cannot be children of the current
        // tile.
        for (auto child_it = std::next(it); child_it != end; child_it++) {
            // Use the actual zoom level for computing the clipping mask.
            const auto& childID = (*child_it)->id;
            leaf.add(TileID { childID.sourceZ, childID.x, childID.y, childID.sourceZ });
        }
        leaf.children.sort();

        // Loop through all existing pools and try to find a matching ClipID.
        auto existing = std::find(pool.begin(), pool.end(), leaf);
        if (existing != pool.end()) {
            leaf.clip = existing->clip;
        } else {
            // We haven't found an existing clip ID
            leaf.clip = {};
            size++;
        }

        pool.emplace_back(std::move(leaf));
    }

    if (size > 0) {
        const uint32_t bit_count = util::ceil_log2(size + 1);
        const std::bitset<8> mask = uint64_t(((1ul << bit_count) - 1) << bit_offset);

        // We are starting our count with 1 since we need at least 1 bit set to distinguish between
        // areas without any tiles whatsoever and the current area.
        uint8_t count = 1;
        for (auto& tile : tiles) {
            tile->clip.mask |= mask;

            // Assign only to clip IDs that have no value yet.
            if (tile->clip.reference.none()) {
                tile->clip.reference = uint32_t(count++) << bit_offset;
            }
        }

        bit_offset += bit_count;
    }

    if (bit_offset > 8) {
        Log::Error(Event::OpenGL, "stencil mask overflow");
    }
}

template <typename Container>
bool coveredByChildren(const TileID& id, const Container& container) {
    for (const auto& child : id.children()) {
        const auto lower = container.lower_bound(child);
        if (lower == container.end() || lower->first.w != child.w ||
            (lower->first != child && !coveredByChildren(child, container))) {
            return false;
        }
    }

    // We looked at all four immediate children and verified that they're covered.
    return true;
}

std::map<TileID, ClipID> ClipIDGenerator::getStencils() const {
    std::map<TileID, ClipID> stencils;

    // Merge everything.
    for (auto& leaf : pool) {
        auto res = stencils.emplace(leaf.id, leaf.clip);
        if (!res.second) {
            // Merge with the existing ClipID when there was already an element with the
            // same tile ID.
            res.first->second |= leaf.clip;
        }
    }

    for (auto it = stencils.begin(); it != stencils.end(); ++it) {
        auto& childId = it->first;
        auto& childClip = it->second;

        // Loop through all preceding stencils, and find all parents.

        for (auto parentIt = std::reverse_iterator<std::map<TileID, ClipID>::iterator>(it);
             parentIt != stencils.rend(); ++parentIt) {
            auto& parentId = parentIt->first;
            if (childId.isChildOf(parentId)) {
                // Once we have a parent, we add the bits  that this ID hasn't set yet.
                const auto& parentClip = parentIt->second;
                const auto mask = ~(childClip.mask & parentClip.mask);
                childClip.reference |= mask & parentClip.reference;
                childClip.mask |= parentClip.mask;
            }
        }
    }

    // Remove tiles that are entirely covered by children.
    util::erase_if(stencils, [&] (const auto& stencil) {
        return coveredByChildren(stencil.first, stencils);
    });

    return stencils;
}

} // namespace mbgl
