#include <mbgl/renderer/sources/render_geojson_source.hpp>
#include <mbgl/renderer/render_tile.hpp>
#include <mbgl/tile/geojson_tile.hpp>

#include <mbgl/algorithm/generate_clip_ids.hpp>
#include <mbgl/algorithm/generate_clip_ids_impl.hpp>

namespace mbgl {

using namespace style;

RenderGeoJSONSource::RenderGeoJSONSource(const style::GeoJSONSource::Impl& impl_)
    : RenderSource(impl_),
      impl(impl_) {
    tilePyramid.setObserver(this);
}

bool RenderGeoJSONSource::isLoaded() const {
    return tilePyramid.isLoaded();
}

void RenderGeoJSONSource::invalidateTiles() {
    tilePyramid.invalidateTiles();
}

void RenderGeoJSONSource::startRender(algorithm::ClipIDGenerator& generator, const mat4& projMatrix, const mat4& clipMatrix, const TransformState& transform) {
    generator.update(tilePyramid.getRenderTiles());
    tilePyramid.startRender(projMatrix, clipMatrix, transform);
}

void RenderGeoJSONSource::finishRender(Painter& painter) {
    tilePyramid.finishRender(painter);
}

std::map<UnwrappedTileID, RenderTile>& RenderGeoJSONSource::getRenderTiles() {
    return tilePyramid.getRenderTiles();
}

void RenderGeoJSONSource::updateTiles(const TileParameters& parameters) {
    GeoJSONData* data_ = impl.getData();

    if (!data_) {
        return;
    }

    if (data_ != data) {
        data = data_;
        tilePyramid.cache.clear();

        for (auto const& item : tilePyramid.tiles) {
            static_cast<GeoJSONTile*>(item.second.get())->updateData(data->getTile(item.first.canonical));
        }
    }

    tilePyramid.updateTiles(parameters,
                            SourceType::GeoJSON,
                            util::tileSize,
                            impl.getZoomRange(),
                            [&] (const OverscaledTileID& tileID) {
                                return std::make_unique<GeoJSONTile>(tileID, impl.id, parameters, data->getTile(tileID.canonical));
                            });
}

void RenderGeoJSONSource::removeTiles() {
    tilePyramid.removeTiles();
}

void RenderGeoJSONSource::reloadTiles() {
    tilePyramid.reloadTiles();
}

std::unordered_map<std::string, std::vector<Feature>>
RenderGeoJSONSource::queryRenderedFeatures(const ScreenLineString& geometry,
                                          const TransformState& transformState,
                                          const RenderedQueryOptions& options) const {
    return tilePyramid.queryRenderedFeatures(geometry, transformState, options);
}

std::vector<Feature> RenderGeoJSONSource::querySourceFeatures(const SourceQueryOptions& options) const {
    return tilePyramid.querySourceFeatures(options);
}

void RenderGeoJSONSource::setCacheSize(size_t size) {
    tilePyramid.setCacheSize(size);
}

void RenderGeoJSONSource::onLowMemory() {
    tilePyramid.onLowMemory();
}

void RenderGeoJSONSource::dumpDebugLogs() const {
    tilePyramid.dumpDebugLogs();
}

} // namespace mbgl
