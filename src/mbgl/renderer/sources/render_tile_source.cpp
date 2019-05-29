#include <mbgl/renderer/sources/render_tile_source.hpp>
#include <mbgl/renderer/render_tile.hpp>
#include <mbgl/renderer/paint_parameters.hpp>
#include <mbgl/tile/vector_tile.hpp>

namespace mbgl {

using namespace style;

RenderTileSource::RenderTileSource(Immutable<style::Source::Impl> impl_)
    : RenderSource(std::move(impl_)) {
    tilePyramid.setObserver(this);
}

RenderTileSource::~RenderTileSource() = default;

bool RenderTileSource::isLoaded() const {
    return tilePyramid.isLoaded();
}

void RenderTileSource::upload(gfx::UploadPass& parameters) {
    tilePyramid.upload(parameters);
}

void RenderTileSource::prepare(const SourcePrepareParameters& parameters) {
    tilePyramid.prepare(parameters);
}

void RenderTileSource::finishRender(PaintParameters& parameters) {
    tilePyramid.finishRender(parameters);
}

void RenderTileSource::updateFadingTiles() {
    tilePyramid.updateFadingTiles();
}

bool RenderTileSource::hasFadingTiles() const {
    return tilePyramid.hasFadingTiles();
}

std::vector<std::reference_wrapper<RenderTile>> RenderTileSource::getRenderTiles() {
    return tilePyramid.getRenderTiles();
}

std::unordered_map<std::string, std::vector<Feature>>
RenderTileSource::queryRenderedFeatures(const ScreenLineString& geometry,
                                          const TransformState& transformState,
                                          const std::vector<const RenderLayer*>& layers,
                                          const RenderedQueryOptions& options,
                                          const mat4& projMatrix) const {
    return tilePyramid.queryRenderedFeatures(geometry, transformState, layers, options, projMatrix);
}

std::vector<Feature> RenderTileSource::querySourceFeatures(const SourceQueryOptions& options) const {
    return tilePyramid.querySourceFeatures(options);
}

void RenderTileSource::reduceMemoryUse() {
    tilePyramid.reduceMemoryUse();
}

void RenderTileSource::dumpDebugLogs() const {
    tilePyramid.dumpDebugLogs();
}

} // namespace mbgl
