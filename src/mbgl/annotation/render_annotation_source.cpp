#include <mbgl/annotation/render_annotation_source.hpp>
#include <mbgl/annotation/annotation_tile.hpp>
#include <mbgl/renderer/render_tile.hpp>
#include <mbgl/renderer/paint_parameters.hpp>

#include <mbgl/layermanager/layer_manager.hpp>

namespace mbgl {

using namespace style;

RenderAnnotationSource::RenderAnnotationSource(Immutable<AnnotationSource::Impl> impl_)
    : RenderTileSource(std::move(impl_)) {
    assert(LayerManager::annotationsEnabled);
    tilePyramid.setObserver(this);
}

const AnnotationSource::Impl& RenderAnnotationSource::impl() const {
    return static_cast<const AnnotationSource::Impl&>(*baseImpl);
}

void RenderAnnotationSource::update(Immutable<style::Source::Impl> baseImpl_,
                                    const std::vector<Immutable<style::LayerProperties>>& layers,
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
                       optional<LatLngBounds> {},
                       [&] (const OverscaledTileID& tileID) {
                           return std::make_unique<AnnotationTile>(tileID, parameters);
                       });
}

std::unordered_map<std::string, std::vector<Feature>>
RenderAnnotationSource::queryRenderedFeatures(const ScreenLineString& geometry,
                                              const TransformState& transformState,
                                              const std::unordered_map<std::string, const RenderLayer*>& layers,
                                              const RenderedQueryOptions& options,
                                              const mat4& projMatrix) const {
    return tilePyramid.queryRenderedFeatures(geometry, transformState, layers, options, projMatrix);
}

std::vector<Feature> RenderAnnotationSource::querySourceFeatures(const SourceQueryOptions&) const {
    return {};
}


} // namespace mbgl
