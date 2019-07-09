#pragma once

#include <mbgl/gfx/texture.hpp>
#include <mbgl/renderer/image_atlas.hpp>
#include <mbgl/style/layer_impl.hpp>

#include <memory>

namespace mbgl {

namespace gfx {
class UploadPass;
} // namespace gfx

class Bucket;
class LayerRenderData;
class SourcePrepareParameters;

class TileAtlasTextures {
public:    
    optional<gfx::Texture> glyph;
    optional<gfx::Texture> icon;
};

class TileRenderData {
public:
    virtual ~TileRenderData();
    const gfx::Texture& getGlyphAtlasTexture() const;
    const gfx::Texture& getIconAtlasTexture() const;
    // To be implemented for concrete tile types.
    virtual optional<ImagePosition> getPattern(const std::string&) const;
    virtual const LayerRenderData* getLayerRenderData(const style::Layer::Impl&) const;
    virtual Bucket* getBucket(const style::Layer::Impl&) const;
    virtual void upload(gfx::UploadPass&) {}
    virtual void prepare(const SourcePrepareParameters&) {}

protected:
    TileRenderData();
    TileRenderData(std::shared_ptr<TileAtlasTextures>);
    std::shared_ptr<TileAtlasTextures> atlasTextures;
};

template <typename BucketType>
class SharedBucketTileRenderData final : public TileRenderData {
public:
    SharedBucketTileRenderData(std::shared_ptr<BucketType> bucket_)
        : bucket(std::move(bucket_)) {}

private:
    // TileRenderData overrides.
    Bucket* getBucket(const style::Layer::Impl&) const override {
        return bucket ? bucket.get() : nullptr;
    }
    void upload(gfx::UploadPass& uploadPass) override {
        if (bucket) bucket->upload(uploadPass);
    }

    std::shared_ptr<BucketType> bucket;
};

} // namespace mbgl
