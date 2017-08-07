#include <mbgl/algorithm/generate_clip_ids_impl.hpp>
#include <mbgl/algorithm/covered_by_children.hpp>

#include <mbgl/util/std.hpp>

#include <list>
#include <vector>
#include <bitset>
#include <cassert>
#include <iostream>
#include <algorithm>
#include <iterator>

namespace mbgl {
namespace algorithm {

ClipIDGenerator::Leaf::Leaf(ClipID& clip_) : clip(clip_) {
}

void ClipIDGenerator::Leaf::add(const CanonicalTileID& p) {
    // Ensure that no already present child is a parent of the new p.
    for (const auto& child : children) {
        if (p.isChildOf(child)) {
            return;
        }
    }
    children.emplace(p);
}

bool ClipIDGenerator::Leaf::operator==(const Leaf& other) const {
    return children == other.children;
}

std::map<UnwrappedTileID, ClipID> ClipIDGenerator::getClipIDs() const {
    std::map<UnwrappedTileID, ClipID> clipIDs;

    // Merge everything.
    for (auto& pair : pool) {
        auto& id = pair.first;
        auto& leaf = pair.second;
        auto res = clipIDs.emplace(id, leaf.clip);
        if (!res.second) {
            // Merge with the existing ClipID when there was already an element with the
            // same tile ID.
            res.first->second |= leaf.clip;
        }
    }

    for (auto it = clipIDs.begin(); it != clipIDs.end(); ++it) {
        auto& childId = it->first;
        auto& childClip = it->second;

        // Loop through all preceding stencils, and find all parents.

        for (auto parentIt = std::reverse_iterator<decltype(it)>(it);
             parentIt != clipIDs.rend(); ++parentIt) {
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
    util::erase_if(clipIDs, [&](const auto& stencil) {
        return algorithm::coveredByChildren(stencil.first, clipIDs);
    });

    return clipIDs;
}

} // namespace algorithm
} // namespace mbgl
