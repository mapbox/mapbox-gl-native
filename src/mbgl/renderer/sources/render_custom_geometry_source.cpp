#include <mbgl/renderer/sources/render_custom_geometry_source.hpp>
#include <mbgl/renderer/render_tile.hpp>
#include <mbgl/renderer/paint_parameters.hpp>
#include <mbgl/tile/custom_geometry_tile.hpp>

#include <mbgl/algorithm/generate_clip_ids.hpp>
#include <mbgl/algorithm/generate_clip_ids_impl.hpp>

namespace mbgl {

using namespace style;

RenderCustomGeometrySource::RenderCustomGeometrySource(Immutable<style::CustomGeometrySource::Impl> impl_)
    : RenderSource(impl_) {
    tilePyramid.setObserver(this);
}

const style::CustomGeometrySource::Impl& RenderCustomGeometrySource::impl() const {
    return static_cast<const style::CustomGeometrySource::Impl&>(*baseImpl);
}

bool RenderCustomGeometrySource::isLoaded() const {
    return tilePyramid.isLoaded();
}

void RenderCustomGeometrySource::update(Immutable<style::Source::Impl> baseImpl_,
                                 const std::vector<Immutable<Layer::Impl>>& layers,
                                 const bool needsRendering,
                                 const bool needsRelayout,
                                 const TileParameters& parameters) {
    std::swap(baseImpl, baseImpl_);

    enabled = needsRendering;

    auto tileLoader = impl().getTileLoader();
    if (!tileLoader) {
        return;
    }

    tilePyramid.update(layers,
                       needsRendering,
                       needsRelayout,
                       parameters,
                       SourceType::CustomVector,
                       util::tileSize,
                       impl().getZoomRange(),
                       {},
                       [&] (const OverscaledTileID& tileID) {
                           return std::make_unique<CustomGeometryTile>(tileID, impl().id, parameters, impl().getTileOptions(), *tileLoader);
                       });
}

void RenderCustomGeometrySource::startRender(PaintParameters& parameters) {
    parameters.clipIDGenerator.update(tilePyramid.getRenderTiles());
    tilePyramid.startRender(parameters);
}

void RenderCustomGeometrySource::finishRender(PaintParameters& parameters) {
    tilePyramid.finishRender(parameters);
}

std::vector<std::reference_wrapper<RenderTile>> RenderCustomGeometrySource::getRenderTiles() {
    return tilePyramid.getRenderTiles();
}

std::unordered_map<std::string, std::vector<Feature>>
RenderCustomGeometrySource::queryRenderedFeatures(const ScreenLineString& geometry,
                                           const TransformState& transformState,
                                           const std::vector<const RenderLayer*>& layers,
                                           const RenderedQueryOptions& options,
                                           const mat4& projMatrix) const {
   return tilePyramid.queryRenderedFeatures(geometry, transformState, layers, options, projMatrix);
}

std::vector<Feature> RenderCustomGeometrySource::querySourceFeatures(const SourceQueryOptions& options) const {
    return tilePyramid.querySourceFeatures(options);
}

void RenderCustomGeometrySource::reduceMemoryUse() {
    tilePyramid.reduceMemoryUse();
}

void RenderCustomGeometrySource::dumpDebugLogs() const {
    tilePyramid.dumpDebugLogs();
}

} // namespace mbgl
