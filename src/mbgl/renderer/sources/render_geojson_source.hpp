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

    void startRender(PaintParameters&) final;
    void finishRender(PaintParameters&) final;

    std::vector<std::reference_wrapper<RenderTile>> getRenderTiles() final;

    std::unordered_map<std::string, std::vector<Feature>>
    queryRenderedFeatures(const ScreenLineString& geometry,
                          const TransformState& transformState,
                          const std::vector<const RenderLayer*>& layers,
                          const RenderedQueryOptions& options,
                          const CollisionIndex&) const final;

    std::vector<Feature>
    querySourceFeatures(const SourceQueryOptions&) const final;

    void reduceMemoryUse() final;
    void dumpDebugLogs() const final;

private:
    const style::GeoJSONSource::Impl& impl() const;

    TilePyramid tilePyramid;
    style::GeoJSONData* data = nullptr;
};

template <>
inline bool RenderSource::is<RenderGeoJSONSource>() const {
    return baseImpl->type == style::SourceType::GeoJSON;
}

} // namespace mbgl
