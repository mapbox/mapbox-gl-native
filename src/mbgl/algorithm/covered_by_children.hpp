#pragma once

#include <mbgl/tile/tile_id.hpp>

namespace mbgl {
namespace algorithm {

template <typename Iterator>
bool coveredByChildren(const UnwrappedTileID& id, Iterator it, const Iterator& end) {
    for (const auto& child : id.children()) {
        it = std::lower_bound(it, end, child, [](const auto& a, const auto& b) { return std::get<0>(a) < b; });

        // Child is not present, neither its grandchildren.
        if (it == end) {
            return false;
        }

        // Child is not present, but its grandchildren are.
        if (std::get<0>(*it) != child) {
            // This child is not covered by its grandchildren.
            if (!coveredByChildren(child, it, end)) {
                return false;
            }
        }
    }

    // We looked at all four children (recursively) and verified that they're covered.
    return true;
}

template <typename Container>
bool coveredByChildren(const UnwrappedTileID& id, const Container& container) {
    return coveredByChildren(
        id, container.upper_bound(id),
        container.lower_bound(UnwrappedTileID{ static_cast<int16_t>(id.wrap + 1), { 0, 0, 0 } }));
}

} // namespace algorithm
} // namespace mbgl
