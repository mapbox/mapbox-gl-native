#pragma once

#include <mbgl/tile/tile_id.hpp>
#include <mbgl/util/range.hpp>

#include <set>

namespace mbgl {
namespace algorithm {

template <typename GetTileFn,
          typename CreateTileFn,
          typename RetainTileFn,
          typename RenderTileFn,
          typename IdealTileIDs>
void updateRenderables(GetTileFn getTile,
                       CreateTileFn createTile,
                       RetainTileFn retainTile,
                       RenderTileFn renderTile,
                       const IdealTileIDs& idealTileIDs,
                       const Range<uint8_t>& zoomRange,
                       const uint8_t dataTileZoom) {
    std::set<UnwrappedTileID> checked;
    bool covered;
    int32_t overscaledZ;

    // for (all in the set of ideal tiles of the source) {
    for (const auto& idealRenderTileID : idealTileIDs) {
        assert(idealRenderTileID.canonical.z >= zoomRange.min);
        assert(idealRenderTileID.canonical.z <= zoomRange.max);
        assert(dataTileZoom >= idealRenderTileID.canonical.z);

        const OverscaledTileID idealDataTileID(dataTileZoom, idealRenderTileID.canonical);
        auto data = getTile(idealDataTileID);
        if (!data) {
            data = createTile(idealDataTileID);
            assert(data);
        }

        // if (source has the tile and bucket is loaded) {
        if (data->isRenderable()) {
            retainTile(*data, true);
            renderTile(idealRenderTileID, *data);
        } else {
            bool triedPrevious = data->hasTriedOptional();

            // The tile isn't loaded yet, but retain it anyway because it's an ideal tile.
            retainTile(*data, true);
            covered = true;
            overscaledZ = dataTileZoom + 1;
            if (overscaledZ > zoomRange.max) {
                // We're looking for an overzoomed child tile.
                const auto childDataTileID = idealDataTileID.scaledTo(overscaledZ);
                data = getTile(childDataTileID);
                if (data && data->isRenderable()) {
                    retainTile(*data, false);
                    renderTile(idealRenderTileID, *data);
                } else {
                    covered = false;
                }
            } else {
                // Check all four actual child tiles.
                for (const auto& childTileID : idealDataTileID.canonical.children()) {
                    const OverscaledTileID childDataTileID(overscaledZ, childTileID);
                    data = getTile(childDataTileID);
                    if (data && data->isRenderable()) {
                        retainTile(*data, false);
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
                for (overscaledZ = dataTileZoom - 1; overscaledZ >= zoomRange.min; --overscaledZ) {
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

                    data = getTile(parentDataTileID);
                    if (!data && triedPrevious) {
                        data = createTile(parentDataTileID);
                    }

                    if (data) {
                        triedPrevious = data->hasTriedOptional();
                        retainTile(*data, false);

                        if (data->isRenderable()) {
                            renderTile(parentRenderTileID, *data);
                            // Break parent tile ascent, since we found one.
                            break;
                        }
                    }
                }
            }
        }
    }
}

} // namespace algorithm
} // namespace mbgl
