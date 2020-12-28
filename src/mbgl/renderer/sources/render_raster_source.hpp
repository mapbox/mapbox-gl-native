#pragma once

#include <mbgl/renderer/sources/render_tile_source.hpp>
#include <mbgl/style/sources/raster_source_impl.hpp>

namespace mbgl {

class RenderRasterSource final : public RenderTileSetSource {
public:
    explicit RenderRasterSource(Immutable<style::RasterSource::Impl>);

private:
    void prepare(const SourcePrepareParameters&) final;

    std::unordered_map<std::string, std::vector<Feature>>
    queryRenderedFeatures(const ScreenLineString& geometry,
                          const TransformState& transformState,
                          const std::unordered_map<std::string, const RenderLayer*>& layers,
                          const RenderedQueryOptions& options,
                          const mat4& projMatrix) const override;

    std::vector<Feature>
    querySourceFeatures(const SourceQueryOptions&) const override;

    // RenderTileSetSource overrides
    void updateInternal(const Tileset&,
                        const std::vector<Immutable<style::LayerProperties>>&,
                        bool needsRendering,
                        bool needsRelayout,
                        const TileParameters&) override;
    const optional<Tileset>& getTileset() const override;

    const style::RasterSource::Impl& impl() const;
};

} // namespace mbgl
