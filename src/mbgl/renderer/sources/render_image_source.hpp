#pragma once

#include <mbgl/renderer/render_source.hpp>
#include <mbgl/renderer/render_tile.hpp>
#include <mbgl/renderer/render_tree.hpp>
#include <mbgl/style/sources/image_source_impl.hpp>

namespace mbgl {

class RasterBucket;

class ImageSourceRenderData {
public:
    ImageSourceRenderData(std::shared_ptr<RasterBucket> bucket_,
                          std::vector<mat4> matrices_)
        : bucket(std::move(bucket_)),
          matrices(std::move(matrices_)) {}
    std::shared_ptr<RasterBucket> bucket;
    std::vector<mat4> matrices;
};

class RenderImageSource final : public RenderSource {
public:
    explicit RenderImageSource(Immutable<style::ImageSource::Impl>);
    ~RenderImageSource() override;

    bool isLoaded() const final;

    void upload(gfx::UploadPass&) final;
    void prepare(const SourcePrepareParameters&) final;
    void finishRender(PaintParameters&) final;
    void updateFadingTiles() final {}
    bool hasFadingTiles() const final { return false; }

    void update(Immutable<style::Source::Impl>,
                const std::vector<Immutable<style::LayerProperties>>&,
                bool needsRendering,
                bool needsRelayout,
                const TileParameters&) final;

    std::vector<std::reference_wrapper<RenderTile>> getRenderTiles() final {
        return {};
    }

    const ImageSourceRenderData* getImageRenderData() const override { 
        return renderData.get();
    }

    std::unordered_map<std::string, std::vector<Feature>>
    queryRenderedFeatures(const ScreenLineString& geometry,
                          const TransformState& transformState,
                          const std::vector<const RenderLayer*>& layers,
                          const RenderedQueryOptions& options,
                          const mat4& projMatrix) const final;

    std::vector<Feature> querySourceFeatures(const SourceQueryOptions&) const final;

    void reduceMemoryUse() final {
    }
    void dumpDebugLogs() const final;

private:
    friend class RenderRasterLayer;
    const style::ImageSource::Impl& impl() const;

    std::shared_ptr<RasterBucket> bucket;
    std::unique_ptr<ImageSourceRenderData> renderData;
    std::vector<UnwrappedTileID> tileIds;
};

template <>
inline bool RenderSource::is<RenderImageSource>() const {
    return baseImpl->type == style::SourceType::Image;
}

} // namespace mbgl
