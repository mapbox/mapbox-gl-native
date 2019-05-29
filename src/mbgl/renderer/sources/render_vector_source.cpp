#include <mbgl/renderer/sources/render_vector_source.hpp>
#include <mbgl/renderer/render_tile.hpp>
#include <mbgl/renderer/paint_parameters.hpp>
#include <mbgl/tile/vector_tile.hpp>

namespace mbgl {

using namespace style;

RenderVectorSource::RenderVectorSource(Immutable<style::VectorSource::Impl> impl_)
    : RenderTileSource(impl_) {
}

const style::VectorSource::Impl& RenderVectorSource::impl() const {
    return static_cast<const style::VectorSource::Impl&>(*baseImpl);
}

void RenderVectorSource::update(Immutable<style::Source::Impl> baseImpl_,
                                const std::vector<Immutable<style::LayerProperties>>& layers,
                                const bool needsRendering,
                                const bool needsRelayout,
                                const TileParameters& parameters) {
    std::swap(baseImpl, baseImpl_);

    enabled = needsRendering;

    optional<Tileset> _tileset = impl().getTileset();

    if (tileset != _tileset) {
        tileset = _tileset;

        // TODO: this removes existing buckets, and will cause flickering.
        // Should instead refresh tile data in place.
        tilePyramid.clearAll();
    }
    // Allow clearing the tile pyramid first, before the early return in case
    //  the new tileset is not yet available or has an error in loading
    if (!_tileset) {
        return;
    }

    tilePyramid.update(layers,
                       needsRendering,
                       needsRelayout,
                       parameters,
                       SourceType::Vector,
                       util::tileSize,
                       tileset->zoomRange,
                       tileset->bounds,
                       [&] (const OverscaledTileID& tileID) {
                           return std::make_unique<VectorTile>(tileID, impl().id, parameters, *tileset);
                       });
}

} // namespace mbgl
