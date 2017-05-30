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
    RenderGeoJSONSource(Immutable<style::GeoJSONSource::Impl>);

    bool isLoaded() const final;

    void update(Immutable<style::Source::Impl>,
                const std::vector<Immutable<style::Layer::Impl>>&,
                bool needsRendering,
                bool needsRelayout,
                const TileParameters&) final;

    void startRender(algorithm::ClipIDGenerator&,
                     const mat4& projMatrix,
                     const mat4& clipMatrix,
                     const TransformState&) final;
    void finishRender(Painter&) final;

    std::map<UnwrappedTileID, RenderTile>& getRenderTiles() final;

    std::unordered_map<std::string, std::vector<Feature>>
    queryRenderedFeatures(const ScreenLineString& geometry,
                          const TransformState& transformState,
                          const style::Style& style,
                          const RenderedQueryOptions& options) const final;

    std::vector<Feature>
    querySourceFeatures(const SourceQueryOptions&) const final;

    void setCacheSize(size_t) final;
    void onLowMemory() final;
    void dumpDebugLogs() const final;

private:
    const style::GeoJSONSource::Impl& impl() const;

    TilePyramid tilePyramid;
    style::GeoJSONData* data;
};

template <>
inline bool RenderSource::is<RenderGeoJSONSource>() const {
    return baseImpl->type == SourceType::GeoJSON;
}

} // namespace mbgl
