#pragma once

#include <mbgl/tile/tile.hpp>
#include <mbgl/tile/tile_id.hpp>
#include <mbgl/tile/tile_necessity.hpp>
#include <mbgl/util/range.hpp>

#include <unordered_set>

#include <iostream>

namespace mbgl {
namespace algorithm {


template <typename GetTileFn,
          typename CreateTileFn,
          typename RetainTileFn,
          typename RenderTileFn>
bool findBestTile(GetTileFn getTile,
                       CreateTileFn createTile,
                       RetainTileFn retainTile,
                       RenderTileFn renderTile,
                       const Range<uint8_t>& zoomRange,
                       const uint8_t dataTileZoom,
                       const UnwrappedTileID& idealRenderTileID,
                       const OverscaledTileID& idealDataTileID,
                       const bool requireFullyRenderable,
                       std::unordered_set<UnwrappedTileID> checked) {

    auto tile = getTile(idealDataTileID);
    if (!tile) {
       // std::cout << "Create tile: " << idealDataTileID << std::endl;
        tile = createTile(idealDataTileID);
        assert(tile);
    }

    bool covered = true;
    std::vector<std::pair<UnwrappedTileID,decltype(tile)>> renderTiles;
    // if (source has the tile and bucket is loaded) {
    if (requireFullyRenderable ? tile->isFullyRenderable() : tile->isRenderable()) {
        retainTile(*tile, TileNecessity::Required);
        renderTiles.emplace_back(std::make_pair(idealRenderTileID, tile));
    } else {
        // We are now attempting to load child and parent tiles.
        bool parentHasTriedOptional = tile->hasTriedCache();
        bool parentIsLoaded = tile->isLoaded();

        // The tile isn't loaded yet, but retain it anyway because it's an ideal tile.
        retainTile(*tile, TileNecessity::Required); // TODO: Is it fine to retain this twice for both full and partial render?

        
        int32_t overscaledZ = dataTileZoom + 1;
        if (overscaledZ > zoomRange.max) {
            // We're looking for an overzoomed child tile.
            const auto childDataTileID = idealDataTileID.scaledTo(overscaledZ);
            tile = getTile(childDataTileID);
            if (tile && (requireFullyRenderable ? tile->isFullyRenderable() : tile->isRenderable())) {
                retainTile(*tile, TileNecessity::Optional);
                renderTiles.emplace_back(std::make_pair(idealRenderTileID, tile));
            } else {
                covered = false;
            }
        } else {
            // Check all four actual child tiles.
            for (const auto& childTileID : idealDataTileID.canonical.children()) {
                const OverscaledTileID childDataTileID(overscaledZ, idealRenderTileID.wrap, childTileID);
                tile = getTile(childDataTileID);
                if (tile && (requireFullyRenderable ? tile->isFullyRenderable() : tile->isRenderable())) {
                    retainTile(*tile, TileNecessity::Optional);
                    renderTiles.emplace_back(std::make_pair(childDataTileID.toUnwrapped(), tile));
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
                const auto parentRenderTileID = parentDataTileID.toUnwrapped();

                if (checked.find(parentRenderTileID) != checked.end()) {
                    // Break parent tile ascent, this route has been checked by another child
                    // tile before.
                    break;
                } else {
                    checked.emplace(parentRenderTileID);
                }

                tile = getTile(parentDataTileID);
                // Don't do cache lookups for parents while we're in the "fully renderable" pass:
                // Since the cache tiles aren't currently displaying, we don't have to worry about symbol flicker
                // And we'd rather get a partially rendered tile closer to our ideal zoom level
                if (!tile && !requireFullyRenderable && (parentHasTriedOptional || parentIsLoaded)) {
                    tile = createTile(parentDataTileID);
                }

                if (tile) {
                    if (!parentIsLoaded) {
                        // We haven't completed loading the child, so we only do an optional
                        // (cache) request in an attempt to quickly load data that we can show.
                        retainTile(*tile, TileNecessity::Optional);
                    } else {
                        // Now that we've checked the child and know for sure that we can't load
                        // it, we attempt to load the parent from the network.
                        retainTile(*tile, TileNecessity::Required);
                    }

                    // Save the current values, since they're the parent of the next iteration
                    // of the parent tile ascent loop.
                    parentHasTriedOptional = tile->hasTriedCache();
                    parentIsLoaded = tile->isLoaded();

                    if ((requireFullyRenderable ? tile->isFullyRenderable() : tile->isRenderable())) {
                        //std::cout << "Rendering parent zoom change: " << parentRenderTileID.canonical.z - idealRenderTileID.canonical.z << std::endl;
                        renderTiles.emplace_back(std::make_pair(parentRenderTileID, tile));
                        covered = true;
                        // Break parent tile ascent, since we found one.
                        break;
                    }
                }
            }
        }
    }
    if (covered || !requireFullyRenderable) {
        // Only add tiles to render tree if we've covered the ideal tile OR we're on our second pass
        for (auto tilePair : renderTiles) {
          //  std::cout << "Rendering tile: " << tilePair.first << std::endl;
            renderTile(tilePair.first, *(tilePair.second));
        }
    } else {
       // std::cout << "No tiles added to render tree" << std::endl;
    }
    return covered;
}

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
    std::unordered_set<UnwrappedTileID> checkedPartial;
    std::unordered_set<UnwrappedTileID> checkedFull;

    //std::cout << "Begin updateRenderables" << std::endl;

    // for (all in the set of ideal tiles of the source) {
    for (const auto& idealRenderTileID : idealTileIDs) {
        assert(idealRenderTileID.canonical.z >= zoomRange.min);
        assert(idealRenderTileID.canonical.z <= zoomRange.max);
        assert(dataTileZoom >= idealRenderTileID.canonical.z);

        const OverscaledTileID idealDataTileID(dataTileZoom, idealRenderTileID.wrap, idealRenderTileID.canonical);
        
        //std::cout << "Ideal tile ID: " << idealDataTileID << std::endl;

        // TODO: Two-pass algorithm isn't necessary for tiles that don't have two stage loading (e.g. raster tiles)
        // TODO: More efficient to write algorithm as single pass?
       if (!findBestTile(getTile, createTile, retainTile, renderTile, zoomRange, dataTileZoom, idealRenderTileID, idealDataTileID, true, checkedFull)) {
            findBestTile(getTile, createTile, retainTile, renderTile, zoomRange, dataTileZoom, idealRenderTileID, idealDataTileID, false, checkedPartial);
       }
    }
}

} // namespace algorithm
} // namespace mbgl
