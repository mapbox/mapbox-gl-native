#pragma once

#include <mbgl/algorithm/generate_clip_ids.hpp>
#include <mbgl/math/log2.hpp>
#include <mbgl/platform/log.hpp>

namespace mbgl {
namespace algorithm {

template <typename Renderables>
void ClipIDGenerator::update(Renderables& renderables) {
    std::size_t size = 0;

    const auto end = renderables.end();
    for (auto it = renderables.begin(); it != end; it++) {
        auto& tileID = it->first;
        auto& renderable = it->second;
        renderable.clip = {};
        Leaf leaf{ renderable.clip };

        // Try to add all remaining ids as children. We sorted the tile list
        // by z earlier, so all preceding items cannot be children of the current
        // tile. We also compute the lower bound of the next wrap, because items of the next wrap
        // can never be children of the current wrap.
        auto child_it = std::next(it);
        const auto children_end = std::lower_bound(
            child_it, end, UnwrappedTileID{ static_cast<int16_t>(tileID.wrap + 1), { 0, 0, 0 } },
            [](auto& a, auto& b) { return a.first < b; });
        for (; child_it != children_end; ++child_it) {
            auto& childTileID = child_it->first;
            if (childTileID.isChildOf(tileID)) {
                leaf.add(childTileID.canonical);
            }
        }

        // Find a leaf with matching children.
        for (auto its = pool.equal_range(tileID); its.first != its.second; ++its.first) {
            auto& existing = its.first->second;
            if (existing == leaf) {
                leaf.clip = existing.clip;
                break;
            }
        }
        if (leaf.clip.reference.none()) {
            // We haven't found an existing clip ID
            size++;
        }

        pool.emplace(tileID, std::move(leaf));
    }

    if (size > 0) {
        const uint32_t bit_count = util::ceil_log2(size + 1);
        const std::bitset<8> mask = uint64_t(((1ul << bit_count) - 1) << bit_offset);

        // We are starting our count with 1 since we need at least 1 bit set to distinguish between
        // areas without any tiles whatsoever and the current area.
        uint8_t count = 1;
        for (auto& pair : renderables) {
            auto& renderable = pair.second;
            renderable.clip.mask |= mask;

            // Assign only to clip IDs that have no value yet.
            if (renderable.clip.reference.none()) {
                renderable.clip.reference = uint32_t(count++) << bit_offset;
            }
        }

        bit_offset += bit_count;
    }

    if (bit_offset > 8) {
        Log::Error(Event::OpenGL, "stencil mask overflow");
    }
}

} // namespace algorithm
} // namespace mbgl
