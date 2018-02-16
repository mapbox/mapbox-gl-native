#pragma once

#include <mbgl/renderer/render_source.hpp>
#include <mbgl/renderer/tile_pyramid.hpp>
#include <mbgl/style/sources/custom_geometry_source_impl.hpp>

namespace mbgl {

class RenderCustomGeometrySource : public RenderSource {
public:
    RenderCustomGeometrySource(Immutable<style::CustomGeometrySource::Impl>);

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
                          const CollisionIndex& collisionIndex) const final;

    std::vector<Feature>
    querySourceFeatures(const SourceQueryOptions&) const final;

    void reduceMemoryUse() final;
    void dumpDebugLogs() const final;
    
private:
    const style::CustomGeometrySource::Impl& impl() const;

    TilePyramid tilePyramid;
};

template <>
inline bool RenderSource::is<RenderCustomGeometrySource>() const {
    return baseImpl->type == style::SourceType::CustomVector;
}

} // namespace mbgl
