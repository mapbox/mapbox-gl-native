#include <mbgl/renderer/sources/render_vector_source.hpp>
#include <mbgl/renderer/render_tile.hpp>
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
                       [&] (const OverscaledTileID& tileID) {
                           return std::make_unique<VectorTile>(tileID, impl().id, parameters, *tileset);
                       });
}

void RenderVectorSource::startRender(algorithm::ClipIDGenerator& generator, const mat4& projMatrix, const mat4& clipMatrix, const TransformState& transform) {
    generator.update(tilePyramid.getRenderTiles());
    tilePyramid.startRender(projMatrix, clipMatrix, transform);
}

void RenderVectorSource::finishRender(Painter& painter) {
    tilePyramid.finishRender(painter);
}

std::map<UnwrappedTileID, RenderTile>& RenderVectorSource::getRenderTiles() {
    return tilePyramid.getRenderTiles();
}

std::unordered_map<std::string, std::vector<Feature>>
RenderVectorSource::queryRenderedFeatures(const ScreenLineString& geometry,
                                          const TransformState& transformState,
                                          const style::Style& style,
                                          const RenderedQueryOptions& options) const {
    return tilePyramid.queryRenderedFeatures(geometry, transformState, style, options);
}

std::vector<Feature> RenderVectorSource::querySourceFeatures(const SourceQueryOptions& options) const {
    return tilePyramid.querySourceFeatures(options);
}

void RenderVectorSource::setCacheSize(size_t size) {
    tilePyramid.setCacheSize(size);
}

void RenderVectorSource::onLowMemory() {
    tilePyramid.onLowMemory();
}

void RenderVectorSource::dumpDebugLogs() const {
    tilePyramid.dumpDebugLogs();
}

} // namespace mbgl
