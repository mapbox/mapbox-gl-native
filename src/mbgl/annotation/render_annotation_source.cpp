#include <mbgl/annotation/render_annotation_source.hpp>
#include <mbgl/annotation/annotation_tile.hpp>
#include <mbgl/renderer/render_tile.hpp>
#include <mbgl/renderer/paint_parameters.hpp>

#include <mbgl/algorithm/generate_clip_ids.hpp>
#include <mbgl/algorithm/generate_clip_ids_impl.hpp>

namespace mbgl {

using namespace style;

RenderAnnotationSource::RenderAnnotationSource(Immutable<AnnotationSource::Impl> impl_)
    : RenderSource(impl_) {
    tilePyramid.setObserver(this);
}

const AnnotationSource::Impl& RenderAnnotationSource::impl() const {
    return static_cast<const AnnotationSource::Impl&>(*baseImpl);
}

bool RenderAnnotationSource::isLoaded() const {
    return tilePyramid.isLoaded();
}

void RenderAnnotationSource::update(Immutable<style::Source::Impl> baseImpl_,
                                    const std::vector<Immutable<Layer::Impl>>& layers,
                                    const bool needsRendering,
                                    const bool needsRelayout,
                                    const TileParameters& parameters) {
    std::swap(baseImpl, baseImpl_);

    enabled = needsRendering;

    tilePyramid.update(layers,
                       needsRendering,
                       needsRelayout,
                       parameters,
                       SourceType::Annotations,
                       util::tileSize,
                       // Zoom level 16 is typically sufficient for annotations.
                       // See https://github.com/mapbox/mapbox-gl-native/issues/10197
                       { 0, 16 },
                       {},
                       [&] (const OverscaledTileID& tileID) {
                           return std::make_unique<AnnotationTile>(tileID, parameters);
                       });
}

void RenderAnnotationSource::startRender(PaintParameters& parameters) {
    parameters.clipIDGenerator.update(tilePyramid.getRenderTiles());
    tilePyramid.startRender(parameters);
}

void RenderAnnotationSource::finishRender(PaintParameters& parameters) {
    tilePyramid.finishRender(parameters);
}

std::vector<std::reference_wrapper<RenderTile>> RenderAnnotationSource::getRenderTiles() {
    return tilePyramid.getRenderTiles();
}

std::unordered_map<std::string, std::vector<Feature>>
RenderAnnotationSource::queryRenderedFeatures(const ScreenLineString& geometry,
                                              const TransformState& transformState,
                                              const std::vector<const RenderLayer*>& layers,
                                              const RenderedQueryOptions& options,
                                              const CollisionIndex& collisionIndex) const {
    return tilePyramid.queryRenderedFeatures(geometry, transformState, layers, options, collisionIndex);
}

std::vector<Feature> RenderAnnotationSource::querySourceFeatures(const SourceQueryOptions&) const {
    return {};
}

void RenderAnnotationSource::onLowMemory() {
    tilePyramid.onLowMemory();
}

void RenderAnnotationSource::dumpDebugLogs() const {
    tilePyramid.dumpDebugLogs();
}

} // namespace mbgl
