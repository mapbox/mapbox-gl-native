#include <mbgl/renderer/sources/render_tile_source.hpp>

#include <mbgl/renderer/buckets/debug_bucket.hpp>
#include <mbgl/renderer/render_tile.hpp>
#include <mbgl/renderer/paint_parameters.hpp>
#include <mbgl/renderer/tile_render_data.hpp>
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
    for (auto& tile : renderTiles) {
        tile.upload(parameters);
    }
}

void RenderTileSource::prepare(const SourcePrepareParameters& parameters) {
    renderTiles.clear();
    renderTiles.reserve(tilePyramid.getRenderedTiles().size());
    for (auto& entry : tilePyramid.getRenderedTiles()) {
        renderTiles.emplace_back(entry.first, entry.second);
        renderTiles.back().prepare(parameters);
    }
}

void RenderTileSource::finishRender(PaintParameters& parameters) {
    for (auto& tile : renderTiles) {
        tile.finishRender(parameters);
    }
}

void RenderTileSource::updateFadingTiles() {
    tilePyramid.updateFadingTiles();
}

bool RenderTileSource::hasFadingTiles() const {
    return tilePyramid.hasFadingTiles();
}

std::vector<std::reference_wrapper<RenderTile>> RenderTileSource::getRenderTiles() {
    return { renderTiles.begin(), renderTiles.end() };
}

const Tile* RenderTileSource::getRenderedTile(const UnwrappedTileID& tileID) const {
    return tilePyramid.getRenderedTile(tileID);
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
