#pragma once

#include <mbgl/renderer/tile_mask.hpp>

#include <vector>
#include <functional>
#include <algorithm>

namespace mbgl {
namespace algorithm {

namespace {

template <typename Renderable>
void computeTileMasks(
    const CanonicalTileID& root,
    const UnwrappedTileID ref,
    typename std::vector<std::reference_wrapper<Renderable>>::const_iterator it,
    const typename std::vector<std::reference_wrapper<Renderable>>::const_iterator end,
    TileMask& mask) {
    // If the reference or any of its children is found in the list, we need to recurse.
    for (; it != end; ++it) {
        auto& renderable = it->get();
        if (!renderable.used) {
            continue;
        }
        if (ref == renderable.id) {
            // The current tile is masked out, so we don't need to add them to the mask set.
            return;
        } else if (renderable.id.isChildOf(ref)) {
            // There's at least one child tile that is masked out, so recursively descend.
            for (const auto& child : ref.children()) {
                computeTileMasks<Renderable>(root, child, it, end, mask);
            }
            return;
        }
    }
    // We couldn't find a child, so it's definitely a masked part.
    mask.emplace(ref.canonical - root);
}

} // namespace

template <typename Renderable>
void updateTileMasks(std::vector<std::reference_wrapper<Renderable>> renderables) {
    std::sort(renderables.begin(), renderables.end(),
              [](const Renderable& a, const Renderable& b) { return a.id < b.id; });

    const auto end = renderables.end();
    for (auto it = renderables.begin(); it != end; it++) {
        auto& renderable = it->get();
        if (!renderable.used) {
            continue;
        }
        // Try to add all remaining ids as children. We sorted the tile list
        // by z earlier, so all preceding items cannot be children of the current
        // tile. We also compute the lower bound of the next wrap, because items of the next wrap
        // can never be children of the current wrap.
        auto child_it = std::next(it);
        const auto children_end = std::lower_bound(
            child_it, end,
            UnwrappedTileID{ static_cast<int16_t>(renderable.id.wrap + 1), { 0, 0, 0 } },
            [](auto& a, auto& b) { return a.get().id < b; });

        renderable.mask.clear();
        computeTileMasks<Renderable>(renderable.id.canonical, renderable.id, child_it, children_end,
                                     renderable.mask);
    }
}

} // namespace algorithm
} // namespace mbgl
