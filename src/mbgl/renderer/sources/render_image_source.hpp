#pragma once

#include <mbgl/renderer/render_source.hpp>
#include <mbgl/renderer/render_tile.hpp>
#include <mbgl/renderer/render_tree.hpp>
#include <mbgl/style/sources/image_source_impl.hpp>

namespace mbgl {

class RasterBucket;

class ImageSourceRenderData final : public RenderItem {
public:
    ImageSourceRenderData(std::shared_ptr<RasterBucket> bucket_,
                          std::vector<mat4> matrices_,
                          std::string name_)
        : bucket(std::move(bucket_)),
          matrices(std::move(matrices_)),
          name(std::move(name_)) {}
    ~ImageSourceRenderData() override;
    const std::shared_ptr<RasterBucket> bucket;
    const std::vector<mat4> matrices;

private:
    void upload(gfx::UploadPass&) const override;
    void render(PaintParameters&) const override;
    bool hasRenderPass(RenderPass) const override { return false; }
    const std::string& getName() const override { return name; }

    std::string name;
};

class RenderImageSource final : public RenderSource {
public:
    explicit RenderImageSource(Immutable<style::ImageSource::Impl>);
    ~RenderImageSource() override;

    bool isLoaded() const final;

    std::unique_ptr<RenderItem> createRenderItem() override;
    void prepare(const SourcePrepareParameters&) final;
    void updateFadingTiles() final {}
    bool hasFadingTiles() const final { return false; }

    void update(Immutable<style::Source::Impl>,
                const std::vector<Immutable<style::LayerProperties>>&,
                bool needsRendering,
                bool needsRelayout,
                const TileParameters&) final;

    const ImageSourceRenderData* getImageRenderData() const override { 
        return renderData.get();
    }

    std::unordered_map<std::string, std::vector<Feature>>
    queryRenderedFeatures(const ScreenLineString& geometry,
                          const TransformState& transformState,
                          const std::unordered_map<std::string, const RenderLayer*>& layers,
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

} // namespace mbgl
