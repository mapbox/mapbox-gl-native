#pragma once

#include <mbgl/renderer/sources/render_tile_source.hpp>
#include <mbgl/style/sources/raster_source_impl.hpp>
#include <mbgl/util/constants.hpp>

namespace mbgl {

class RenderRasterDEMSource final : public RenderTileSource {
public:
    explicit RenderRasterDEMSource(Immutable<style::RasterSource::Impl>);

    void update(Immutable<style::Source::Impl>,
                const std::vector<Immutable<style::LayerProperties>>&,
                bool needsRendering,
                bool needsRelayout,
                const TileParameters&) final;

    void prepare(const SourcePrepareParameters&) override;

    std::unordered_map<std::string, std::vector<Feature>>
    queryRenderedFeatures(const ScreenLineString& geometry,
                          const TransformState& transformState,
                          const std::vector<const RenderLayer*>& layers,
                          const RenderedQueryOptions& options,
                          const mat4& projMatrix) const override;

    std::vector<Feature>
    querySourceFeatures(const SourceQueryOptions&) const override;

    uint8_t getMaxZoom() const { return maxzoom; }

private:
    const style::RasterSource::Impl& impl() const;

    optional<Tileset> tileset;
    uint8_t maxzoom = util::TERRAIN_RGB_MAXZOOM;

    void onTileChanged(Tile&) override;
};

template <>
inline bool RenderSource::is<RenderRasterDEMSource>() const {
    return baseImpl->type == style::SourceType::RasterDEM;
}

} // namespace mbgl
