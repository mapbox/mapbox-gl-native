#pragma once

#include <mbgl/renderer/render_source.hpp>
#include <mbgl/renderer/tile_pyramid.hpp>
#include <mbgl/style/sources/geojson_source_impl.hpp>

namespace mbgl {

namespace style {
class GeoJSONData;
} // namespace style

class RenderGeoJSONSource : public RenderSource {
public:
    RenderGeoJSONSource(const style::GeoJSONSource::Impl&);

    bool isLoaded() const final;

    // Called when the camera has changed. May load new tiles, unload obsolete tiles, or
    // trigger re-placement of existing complete tiles.
    void updateTiles(const TileParameters&) final;

    // Removes all tiles (by putting them into the cache).
    void removeTiles() final;

    // Remove all tiles and clear the cache.
    void invalidateTiles() final;

    // Request that all loaded tiles re-run the layout operation on the existing source
    // data with fresh style information.
    void reloadTiles() final;

    void startRender(algorithm::ClipIDGenerator&,
                     const mat4& projMatrix,
                     const mat4& clipMatrix,
                     const TransformState&) final;
    void finishRender(Painter&) final;

    std::map<UnwrappedTileID, RenderTile>& getRenderTiles() final;

    std::unordered_map<std::string, std::vector<Feature>>
    queryRenderedFeatures(const ScreenLineString& geometry,
                          const TransformState& transformState,
                          const RenderedQueryOptions& options) const final;

    std::vector<Feature>
    querySourceFeatures(const SourceQueryOptions&) const final;

    void setCacheSize(size_t) final;
    void onLowMemory() final;
    void dumpDebugLogs() const final;

private:
    const style::GeoJSONSource::Impl& impl;
    TilePyramid tilePyramid;
    style::GeoJSONData* data;
};

} // namespace mbgl
