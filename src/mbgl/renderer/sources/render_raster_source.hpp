#pragma once

#include <mbgl/renderer/render_source.hpp>
#include <mbgl/renderer/tile_pyramid.hpp>
#include <mbgl/style/sources/raster_source_impl.hpp>

namespace mbgl {

class RenderRasterSource : public RenderSource {
public:
    RenderRasterSource(Immutable<style::RasterSource::Impl>);

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
                          const RenderStyle& style,
                          const RenderedQueryOptions& options) const final;

    std::vector<Feature>
    querySourceFeatures(const SourceQueryOptions&) const final;

    void setCacheSize(size_t) final;
    void onLowMemory() final;
    void dumpDebugLogs() const final;

private:
    const style::RasterSource::Impl& impl() const;

    TilePyramid tilePyramid;
    optional<std::vector<std::string>> tileURLTemplates;
};

template <>
inline bool RenderSource::is<RenderRasterSource>() const {
    return baseImpl->type == SourceType::Raster;
}

} // namespace mbgl
