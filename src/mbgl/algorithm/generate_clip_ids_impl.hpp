#pragma once

#include <mbgl/algorithm/generate_clip_ids.hpp>
#include <mbgl/math/log2.hpp>
#include <mbgl/util/logging.hpp>

namespace mbgl {
namespace algorithm {

template <typename Renderable>
void ClipIDGenerator::update(std::vector<std::reference_wrapper<Renderable>> renderables) {
    std::size_t size = 0;

    std::sort(renderables.begin(), renderables.end(),
              [](const auto& a, const auto& b) { return a.get().id < b.get().id; });

    const auto end = renderables.end();
    for (auto it = renderables.begin(); it != end; it++) {
        auto& renderable = it->get();
        if (!renderable.used || !renderable.needsClipping) {
            continue;
        }

        renderable.clip = {};
        Leaf leaf{ renderable.clip };

        // Try to add all remaining ids as children. We sorted the tile list
        // by z earlier, so all preceding items cannot be children of the current
        // tile. We also compute the lower bound of the next wrap, because items of the next wrap
        // can never be children of the current wrap.
        auto child_it = std::next(it);
        const auto children_end = std::lower_bound(
            child_it, end, UnwrappedTileID{ static_cast<int16_t>(renderable.id.wrap + 1), { 0, 0, 0 } },
            [](auto& a, auto& b) { return a.get().id < b; });
        for (; child_it != children_end; ++child_it) {
            auto& childTileID = child_it->get().id;
            if (childTileID.isChildOf(it->get().id)) {
                leaf.add(childTileID.canonical);
            }
        }

        // Find a leaf with matching children.
        for (auto its = pool.equal_range(renderable.id); its.first != its.second; ++its.first) {
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

        pool.emplace(renderable.id, std::move(leaf));
    }

    if (size > 0) {
        const uint32_t bit_count = util::ceil_log2(size + 1);
        const std::bitset<8> mask = uint64_t(((1ul << bit_count) - 1) << bit_offset);

        // We are starting our count with 1 since we need at least 1 bit set to distinguish between
        // areas without any tiles whatsoever and the current area.
        uint8_t count = 1;
        for (auto& it : renderables) {
            auto& renderable = it.get();
            if (!renderable.used) {
                continue;
            }
            renderable.clip.mask |= mask;

            // Assign only to clip IDs that have no value yet.
            if (renderable.clip.reference.none()) {
                renderable.clip.reference = uint32_t(count++) << bit_offset;
            }
        }

        bit_offset += bit_count;
    }

    // Prevent this warning from firing on every frame,
    // which can be expensive in some platforms.
    static bool warned = false;

    if (!warned && bit_offset > 8) {
        Log::Error(Event::OpenGL, "stencil mask overflow");
        warned = true;
    }
}

} // namespace algorithm
} // namespace mbgl
