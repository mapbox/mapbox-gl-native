#include <mbgl/annotation/render_annotation_source.hpp>
#include <mbgl/annotation/annotation_tile.hpp>
#include <mbgl/renderer/render_tile.hpp>

#include <mbgl/algorithm/generate_clip_ids.hpp>
#include <mbgl/algorithm/generate_clip_ids_impl.hpp>

namespace mbgl {

using namespace style;

RenderAnnotationSource::RenderAnnotationSource(const AnnotationSource::Impl& impl_)
    : RenderSource(impl_) {
    tilePyramid.setObserver(this);
}

bool RenderAnnotationSource::isLoaded() const {
    return tilePyramid.isLoaded();
}

void RenderAnnotationSource::invalidateTiles() {
    tilePyramid.invalidateTiles();
}

void RenderAnnotationSource::startRender(algorithm::ClipIDGenerator& generator, const mat4& projMatrix, const mat4& clipMatrix, const TransformState& transform) {
    generator.update(tilePyramid.getRenderTiles());
    tilePyramid.startRender(projMatrix, clipMatrix, transform);
}

void RenderAnnotationSource::finishRender(Painter& painter) {
    tilePyramid.finishRender(painter);
}

std::map<UnwrappedTileID, RenderTile>& RenderAnnotationSource::getRenderTiles() {
    return tilePyramid.getRenderTiles();
}

void RenderAnnotationSource::updateTiles(const TileParameters& parameters) {
    tilePyramid.updateTiles(parameters,
                            SourceType::Annotations,
                            util::tileSize,
                            { 0, 22 },
                            [&] (const OverscaledTileID& tileID) {
                                return std::make_unique<AnnotationTile>(tileID, parameters);
                            });
}

void RenderAnnotationSource::removeTiles() {
    tilePyramid.removeTiles();
}

void RenderAnnotationSource::reloadTiles() {
    tilePyramid.reloadTiles();
}

std::unordered_map<std::string, std::vector<Feature>>
RenderAnnotationSource::queryRenderedFeatures(const ScreenLineString& geometry,
                                          const TransformState& transformState,
                                          const RenderedQueryOptions& options) const {
    return tilePyramid.queryRenderedFeatures(geometry, transformState, options);
}

std::vector<Feature> RenderAnnotationSource::querySourceFeatures(const SourceQueryOptions&) const {
    return {};
}

void RenderAnnotationSource::setCacheSize(size_t size) {
    tilePyramid.setCacheSize(size);
}

void RenderAnnotationSource::onLowMemory() {
    tilePyramid.onLowMemory();
}

void RenderAnnotationSource::dumpDebugLogs() const {
    tilePyramid.dumpDebugLogs();
}

} // namespace mbgl
