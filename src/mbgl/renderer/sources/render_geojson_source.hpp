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
    ~RenderGeoJSONSource() final;

    bool isLoaded() const final;

    void update(Immutable<style::Source::Impl>,
                const std::vector<Immutable<style::LayerProperties>>&,
                bool needsRendering,
                bool needsRelayout,
                const TileParameters&) final;

    void upload(gfx::UploadPass&) final;
    void prepare(PaintParameters&) final;
    void finishRender(PaintParameters&) final;

    std::vector<std::reference_wrapper<RenderTile>> getRenderTiles() final;

    std::unordered_map<std::string, std::vector<Feature>>
    queryRenderedFeatures(const ScreenLineString& geometry,
                          const TransformState& transformState,
                          const std::vector<const RenderLayer*>& layers,
                          const RenderedQueryOptions& options,
                          const mat4& projMatrix) const final;

    std::vector<Feature>
    querySourceFeatures(const SourceQueryOptions&) const final;

    FeatureExtensionValue
    queryFeatureExtensions(const Feature& feature,
                           const std::string& extension,
                           const std::string& extensionField,
                           const optional<std::map<std::string, Value>>& args) const final;

    void reduceMemoryUse() final;
    void dumpDebugLogs() const final;

private:
    const style::GeoJSONSource::Impl& impl() const;

    TilePyramid tilePyramid;
    std::weak_ptr<style::GeoJSONData> data;
};

template <>
inline bool RenderSource::is<RenderGeoJSONSource>() const {
    return baseImpl->type == style::SourceType::GeoJSON;
}

} // namespace mbgl
