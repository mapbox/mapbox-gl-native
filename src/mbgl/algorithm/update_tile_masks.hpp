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
    // Compute the difference between the root tile ID and the reference tile ID, since TileMask
    // elements are always relative (see below for explanation).
    const uint8_t diffZ = ref.canonical.z - root.z;
    mask.emplace(diffZ, ref.canonical.x - (root.x << diffZ), ref.canonical.y - (root.y << diffZ));
}

} // namespace

// Updates the TileMasks for all renderables. Renderables are objects that have an UnwrappedTileID
// property indicating where they should be rendered on the screen. A TileMask describes all regions
// within that tile that are *not* covered by other Renderables.
// Example: Renderables in our list are 2/1/3, 3/3/6, and 4/5/13. The schematic for creating the
// TileMask for 2/1/3 looks like this:
//
//    ┌────────┬────────┬─────────────────┐
//    │        │        │#################│
//    │ 4/4/12 │ 4/5/12 │#################│
//    │        │        │#################│
//    ├──────3/2/6──────┤#####3/3/6#######│
//    │        │########│#################│
//    │ 4/4/13 │#4/5/13#│#################│
//    │        │########│#################│
//    ├────────┴──────2/1/3───────────────┤
//    │                 │                 │
//    │                 │                 │
//    │                 │                 │
//    │      3/2/7      │      3/3/7      │
//    │                 │                 │
//    │                 │                 │
//    │                 │                 │
//    └─────────────────┴─────────────────┘
//
// The TileMask for 2/1/3 thus consists of the tiles 4/4/12, 4/5/12, 4/4/13, 3/2/7, and 3/3/7,
// but it does *not* include 4/5/13, and 3/3/6, since these are other Renderables.
// A TileMask always contains TileIDs *relative* to the tile it is generated for, so 2/1/3 is
// "subtracted" from these TileIDs. The final TileMask for 2/1/3 will thus be:
//
//    ┌────────┬────────┬─────────────────┐
//    │        │        │#################│
//    │ 2/0/0  │ 2/1/0  │#################│
//    │        │        │#################│
//    ├────────┼────────┤#################│
//    │        │########│#################│
//    │ 2/0/1  │########│#################│
//    │        │########│#################│
//    ├────────┴────────┼─────────────────┤
//    │                 │                 │
//    │                 │                 │
//    │                 │                 │
//    │      1/0/1      │      1/1/1      │
//    │                 │                 │
//    │                 │                 │
//    │                 │                 │
//    └─────────────────┴─────────────────┘
//
// Only other Renderables that are *children* of the Renderable we are generating the mask for will
// be considered. For example, adding a Renderable with TileID 4/8/13 won't affect the TileMask for
// 2/1/3, since it is not a descendant of it.
template <typename Renderable>
void updateTileMasks(std::vector<std::reference_wrapper<Renderable>> renderables) {
    std::sort(renderables.begin(), renderables.end(),
              [](const Renderable& a, const Renderable& b) { return a.id < b.id; });

    TileMask mask;
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

        mask.clear();
        computeTileMasks<Renderable>(renderable.id.canonical, renderable.id, child_it, children_end,
                                     mask);
        renderable.setMask(std::move(mask));
    }
}

} // namespace algorithm
} // namespace mbgl
