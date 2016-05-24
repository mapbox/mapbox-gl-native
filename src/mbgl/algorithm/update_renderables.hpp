#pragma once

#include <mbgl/tile/tile_id.hpp>

#include <set>

namespace mbgl {
namespace algorithm {

template <typename GetTileDataFn,
          typename CreateTileDataFn,
          typename RetainTileDataFn,
          typename RenderTileFn,
          typename IdealTileIDs,
          typename SourceInfo>
void updateRenderables(GetTileDataFn getTileData,
                       CreateTileDataFn createTileData,
                       RetainTileDataFn retainTileData,
                       RenderTileFn renderTile,
                       const IdealTileIDs& idealTileIDs,
                       const SourceInfo& info,
                       const uint8_t dataTileZoom) {
    std::set<UnwrappedTileID> checked;
    bool covered;
    int32_t overscaledZ;

    // for (all in the set of ideal tiles of the source) {
    for (const auto& idealRenderTileID : idealTileIDs) {
        assert(idealRenderTileID.canonical.z >= info.minZoom);
        assert(idealRenderTileID.canonical.z <= info.maxZoom);
        assert(dataTileZoom >= idealRenderTileID.canonical.z);

        const OverscaledTileID idealDataTileID(dataTileZoom, idealRenderTileID.canonical);
        auto data = getTileData(idealDataTileID);
        if (!data) {
            data = createTileData(idealDataTileID);
            assert(data);
        }

        // if (source has the tile and bucket is loaded) {
        if (data->isRenderable()) {
            retainTileData(*data);
            renderTile(idealRenderTileID, *data);
        } else {
            // The tile isn't loaded yet, but retain it anyway because it's an ideal tile.
            retainTileData(*data);
            covered = true;
            overscaledZ = dataTileZoom + 1;
            if (overscaledZ > info.maxZoom) {
                // We're looking for an overzoomed child tile.
                const auto childDataTileID = idealDataTileID.scaledTo(overscaledZ);
                data = getTileData(childDataTileID);
                if (data && data->isRenderable()) {
                    retainTileData(*data);
                    renderTile(idealRenderTileID, *data);
                } else {
                    covered = false;
                }
            } else {
                // Check all four actual child tiles.
                for (const auto& childTileID : idealDataTileID.canonical.children()) {
                    const OverscaledTileID childDataTileID(overscaledZ, childTileID);
                    data = getTileData(childDataTileID);
                    if (data && data->isRenderable()) {
                        retainTileData(*data);
                        renderTile(childDataTileID.unwrapTo(idealRenderTileID.wrap), *data);
                    } else {
                        // At least one child tile doesn't exist, so we are going to look for
                        // parents as well.
                        covered = false;
                    }
                }
            }

            if (!covered) {
                // We couldn't find child tiles that entirely cover the ideal tile.
                for (overscaledZ = dataTileZoom - 1; overscaledZ >= info.minZoom; --overscaledZ) {
                    const auto parentDataTileID = idealDataTileID.scaledTo(overscaledZ);
                    const auto parentRenderTileID =
                        parentDataTileID.unwrapTo(idealRenderTileID.wrap);

                    if (checked.find(parentRenderTileID) != checked.end()) {
                        // Break parent tile ascent, this route has been checked by another child
                        // tile before.
                        break;
                    } else {
                        checked.emplace(parentRenderTileID);
                    }

                    data = getTileData(parentDataTileID);
                    if (data && data->isRenderable()) {
                        retainTileData(*data);
                        renderTile(parentRenderTileID, *data);
                        // Break parent tile ascent, since we found one.
                        break;
                    }
                }
            }
        }
    }
}

} // namespace algorithm
} // namespace mbgl
