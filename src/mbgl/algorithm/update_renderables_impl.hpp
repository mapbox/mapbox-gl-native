#pragma once

#include <mbgl/algorithm/update_renderables.hpp>
#include <mbgl/tile/tile_id.hpp>

#include <map>

namespace mbgl {
namespace algorithm {

namespace {

template <typename DataTiles, typename Renderables>
bool tryTile(const UnwrappedTileID& renderTileID,
             const OverscaledTileID& dataTileID,
             const DataTiles& dataTiles,
             Renderables& renderables) {
    if (renderables.find(renderTileID) == renderables.end()) {
        const auto it = dataTiles.find(dataTileID);
        if (it == dataTiles.end() || !it->second->isReady()) {
            return false;
        }

        using Renderable = typename Renderables::mapped_type;
        renderables.emplace(renderTileID, Renderable{ renderTileID, *it->second });
    }

    return true;
}

} // namespace

template <typename Renderable, typename DataTiles, typename IdealTileIDs, typename SourceInfo>
std::map<UnwrappedTileID, Renderable> updateRenderables(const DataTiles& dataTiles,
                                                        const IdealTileIDs& idealTileIDs,
                                                        const SourceInfo& info,
                                                        const uint8_t z) {
    std::map<UnwrappedTileID, Renderable> renderables;

    // for (all in the set of ideal tiles of the source) {
    for (const auto& renderTileID : idealTileIDs) {
        assert(renderTileID.canonical.z >= info.minZoom);
        assert(renderTileID.canonical.z <= info.maxZoom);
        assert(z >= renderTileID.canonical.z);
        const auto wrap = renderTileID.wrap;
        const OverscaledTileID dataTileID(z, renderTileID.canonical);

        // if (source has the tile and bucket is loaded) {
        if (!tryTile(renderTileID, dataTileID, dataTiles, renderables)) {
            // The source doesn't have the tile, or the bucket isn't loaded.
            bool covered = true;
            int32_t overscaledZ = z + 1;
            if (overscaledZ > info.maxZoom) {
                // We're looking for an overzoomed child tile.
                const auto childDataTileID = dataTileID.scaledTo(overscaledZ);
                if (!tryTile(renderTileID, childDataTileID, dataTiles, renderables)) {
                    covered = false;
                }
            } else {
                // Check all four actual child tiles.
                for (const auto& childTileID : dataTileID.canonical.children()) {
                    const OverscaledTileID childDataTileID(overscaledZ, childTileID);
                    const UnwrappedTileID childRenderTileID(wrap, childTileID);
                    if (!tryTile(childRenderTileID, childDataTileID, dataTiles, renderables)) {
                        // At least one child tile doesn't exist, so we are going to look for
                        // parents as well.
                        covered = false;
                    }
                }
            }

            if (!covered) {
                // We couldn't find child tiles that entirely cover the ideal tile.
                for (overscaledZ = z - 1; overscaledZ >= info.minZoom; --overscaledZ) {
                    const auto parentDataTileID = dataTileID.scaledTo(overscaledZ);
                    const auto parentRenderTileID = parentDataTileID.unwrapTo(renderTileID.wrap);
                    if (tryTile(parentRenderTileID, parentDataTileID, dataTiles, renderables)) {
                        // Break parent tile ascent, since we found one.
                        break;
                    }
                }
            }
        }
    }

    return renderables;
}

} // namespace algorithm
} // namespace mbgl
