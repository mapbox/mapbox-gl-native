#pragma once

#include <mbgl/renderer/render_source.hpp>
#include <mbgl/renderer/render_tile.hpp>
#include <mbgl/style/sources/image_source_impl.hpp>

namespace mbgl {
class RenderLayer;
class PaintParameters;
class RasterBucket;

namespace gl {
class Context;
} // namespace gl

class RenderImageSource : public RenderSource {
public:
    RenderImageSource(Immutable<style::ImageSource::Impl>);
    ~RenderImageSource() override;

    bool isLoaded() const final;
    void upload(gl::Context&);

    void startRender(algorithm::ClipIDGenerator&,
                     const mat4& projMatrix,
                     const mat4& clipMatrix,
                     const TransformState&) final;

    void render(Painter&, PaintParameters&, const RenderLayer&);

    void finishRender(Painter&) final;

    void update(Immutable<style::Source::Impl>,
                const std::vector<Immutable<style::Layer::Impl>>&,
                bool needsRendering,
                bool needsRelayout,
                const TileParameters&) final;

    std::map<UnwrappedTileID, RenderTile>& getRenderTiles() final {
        return tiles;
    }

    std::unordered_map<std::string, std::vector<Feature>>
    queryRenderedFeatures(const ScreenLineString& geometry,
                          const TransformState& transformState,
                          const style::Style& style,
                          const RenderedQueryOptions& options) const final;

    std::vector<Feature> querySourceFeatures(const SourceQueryOptions&) const final;

    void setCacheSize(size_t) final {
    }
    void onLowMemory() final {
    }
    void dumpDebugLogs() const final;

private:
    const style::ImageSource::Impl& impl() const;
    std::map<UnwrappedTileID, RenderTile> tiles;

    std::vector<UnwrappedTileID> tileIds;
    std::unique_ptr<RasterBucket> bucket;
    std::vector<mat4> matrices;
    bool shouldRender;
};

template <>
inline bool RenderSource::is<RenderImageSource>() const {
    return baseImpl->type == SourceType::Image;
}

} // namespace mbgl
