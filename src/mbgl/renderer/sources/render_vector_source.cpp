#include <mbgl/renderer/sources/render_vector_source.hpp>
#include <mbgl/renderer/render_tile.hpp>
#include <mbgl/renderer/paint_parameters.hpp>
#include <mbgl/tile/vector_tile.hpp>

#include <mbgl/algorithm/generate_clip_ids.hpp>
#include <mbgl/algorithm/generate_clip_ids_impl.hpp>

namespace mbgl {

using namespace style;

RenderVectorSource::RenderVectorSource(Immutable<style::VectorSource::Impl> impl_)
    : RenderSource(impl_) {
    tilePyramid.setObserver(this);
}

const style::VectorSource::Impl& RenderVectorSource::impl() const {
    return static_cast<const style::VectorSource::Impl&>(*baseImpl);
}

bool RenderVectorSource::isLoaded() const {
    return tilePyramid.isLoaded();
}

void RenderVectorSource::update(Immutable<style::Source::Impl> baseImpl_,
                                const std::vector<Immutable<Layer::Impl>>& layers,
                                const bool needsRendering,
                                const bool needsRelayout,
                                const TileParameters& parameters) {
    std::swap(baseImpl, baseImpl_);

    enabled = needsRendering;

    optional<Tileset> tileset = impl().getTileset();

    if (!tileset) {
        return;
    }

    if (tileURLTemplates != tileset->tiles) {
        tileURLTemplates = tileset->tiles;

        // TODO: this removes existing buckets, and will cause flickering.
        // Should instead refresh tile data in place.
        tilePyramid.tiles.clear();
        tilePyramid.renderTiles.clear();
        tilePyramid.cache.clear();
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

void RenderVectorSource::startRender(PaintParameters& parameters) {
    parameters.clipIDGenerator.update(tilePyramid.getRenderTiles());
    tilePyramid.startRender(parameters);
}

void RenderVectorSource::finishRender(PaintParameters& parameters) {
    tilePyramid.finishRender(parameters);
}

std::vector<std::reference_wrapper<RenderTile>> RenderVectorSource::getRenderTiles() {
    return tilePyramid.getRenderTiles();
}

std::unordered_map<std::string, std::vector<Feature>>
RenderVectorSource::queryRenderedFeatures(const ScreenLineString& geometry,
                                          const TransformState& transformState,
                                          const std::vector<const RenderLayer*>& layers,
                                          const RenderedQueryOptions& options,
                                          const CollisionIndex& collisionIndex) const {
    return tilePyramid.queryRenderedFeatures(geometry, transformState, layers, options, collisionIndex);
}

std::vector<Feature> RenderVectorSource::querySourceFeatures(const SourceQueryOptions& options) const {
    return tilePyramid.querySourceFeatures(options);
}

void RenderVectorSource::onLowMemory() {
    tilePyramid.onLowMemory();
}

void RenderVectorSource::dumpDebugLogs() const {
    tilePyramid.dumpDebugLogs();
}

} // namespace mbgl
