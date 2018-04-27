#pragma once

#include <mbgl/renderer/render_source.hpp>
#include <mbgl/renderer/tile_pyramid.hpp>
#include <mbgl/style/sources/vector_source_impl.hpp>

namespace mbgl {

class RenderVectorSource : public RenderSource {
public:
    RenderVectorSource(Immutable<style::VectorSource::Impl>);

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
                          const mat4& projMatrix) const final;

    std::vector<Feature>
    querySourceFeatures(const SourceQueryOptions&) const final;

    void reduceMemoryUse() final;
    void dumpDebugLogs() const final;

private:
    const style::VectorSource::Impl& impl() const;

    TilePyramid tilePyramid;
    optional<Tileset> tileset;
};

template <>
inline bool RenderSource::is<RenderVectorSource>() const {
    return baseImpl->type == style::SourceType::Vector;
}

} // namespace mbgl
