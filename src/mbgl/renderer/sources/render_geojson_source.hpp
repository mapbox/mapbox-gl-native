#pragma once

#include <mbgl/renderer/sources/render_tile_source.hpp>
#include <mbgl/style/sources/geojson_source_impl.hpp>

namespace mbgl {

namespace style {
class GeoJSONData;
} // namespace style

class RenderGeoJSONSource final : public RenderTileSource {
public:
    explicit RenderGeoJSONSource(Immutable<style::GeoJSONSource::Impl>);
    ~RenderGeoJSONSource() override;

    void update(Immutable<style::Source::Impl>,
                const std::vector<Immutable<style::LayerProperties>>&,
                bool needsRendering,
                bool needsRelayout,
                const TileParameters&) override;

    FeatureExtensionValue
    queryFeatureExtensions(const Feature& feature,
                           const std::string& extension,
                           const std::string& extensionField,
                           const optional<std::map<std::string, Value>>& args) const override;

private:
    const style::GeoJSONSource::Impl& impl() const;

    std::weak_ptr<style::GeoJSONData> data;
};

} // namespace mbgl
