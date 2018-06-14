#pragma once

#include <mbgl/tile/tile.hpp>
#include <mbgl/tile/geometry_tile_worker.hpp>
#include <mbgl/renderer/image_manager.hpp>
#include <mbgl/text/glyph_manager.hpp>
#include <mbgl/util/feature.hpp>
#include <mbgl/actor/actor.hpp>
#include <mbgl/geometry/feature_index.hpp>

#include <atomic>
#include <memory>
#include <unordered_map>
#include <vector>

namespace mbgl {

class GeometryTileData;
class RenderLayer;
class SourceQueryOptions;
class TileParameters;
class GlyphAtlas;
class ImageAtlas;

class GeometryTile : public Tile, public GlyphRequestor, ImageRequestor {
public:
    GeometryTile(const OverscaledTileID&,
                 std::string sourceID,
                 const TileParameters&);

    ~GeometryTile() override;

    void setError(std::exception_ptr);
    void setData(std::unique_ptr<const GeometryTileData>);

    void setLayers(const std::vector<Immutable<style::Layer::Impl>>&) override;
    void setShowCollisionBoxes(const bool showCollisionBoxes) override;

    void onGlyphsAvailable(GlyphMap) override;
    void onImagesAvailable(ImageMap, ImageMap, uint64_t imageCorrelationID) override;
    
    void getGlyphs(GlyphDependencies);
    void getImages(ImageRequestPair);

    void upload(gl::Context&) override;
    Bucket* getBucket(const style::Layer::Impl&) const override;

    Size bindGlyphAtlas(gl::Context&);
    Size bindIconAtlas(gl::Context&);

    void queryRenderedFeatures(
            std::unordered_map<std::string, std::vector<Feature>>& result,
            const GeometryCoordinates& queryGeometry,
            const TransformState&,
            const std::vector<const RenderLayer*>& layers,
            const RenderedQueryOptions& options,
            const mat4& projMatrix) override;

    void querySourceFeatures(
        std::vector<Feature>& result,
        const SourceQueryOptions&) override;

    float getQueryPadding(const std::vector<const RenderLayer*>&) override;

    void cancel() override;

    class LayoutResult {
    public:
        std::unordered_map<std::string, std::shared_ptr<Bucket>> buckets;
        std::unique_ptr<FeatureIndex> featureIndex;
        optional<AlphaImage> glyphAtlasImage;
        ImageAtlas iconAtlas;

        LayoutResult(std::unordered_map<std::string, std::shared_ptr<Bucket>> buckets_,
                     std::unique_ptr<FeatureIndex> featureIndex_,
                     optional<AlphaImage> glyphAtlasImage_,
                     ImageAtlas iconAtlas_)
            : buckets(std::move(buckets_)),
              featureIndex(std::move(featureIndex_)),
              glyphAtlasImage(std::move(glyphAtlasImage_)),
              iconAtlas(std::move(iconAtlas_)) {}
    };
    void onLayout(LayoutResult, uint64_t correlationID);

    void onError(std::exception_ptr, uint64_t correlationID);

    bool holdForFade() const override;
    void markRenderedIdeal() override;
    void markRenderedPreviously() override;
    void performedFadePlacement() override;
    const optional<ImagePosition> getPattern(const std::string& pattern);
    const std::shared_ptr<FeatureIndex> getFeatureIndex() const { return latestFeatureIndex; }
    
protected:
    const GeometryTileData* getData() {
        return latestFeatureIndex ? latestFeatureIndex->getData() : nullptr;
    }

private:
    void markObsolete();

    const std::string sourceID;

    // Used to signal the worker that it should abandon parsing this tile as soon as possible.
    std::atomic<bool> obsolete { false };

    std::shared_ptr<Mailbox> mailbox;
    Actor<GeometryTileWorker> worker;

    GlyphManager& glyphManager;
    ImageManager& imageManager;

    uint64_t correlationID = 0;

    std::unordered_map<std::string, std::shared_ptr<Bucket>> buckets;
    
    std::shared_ptr<FeatureIndex> latestFeatureIndex;

    optional<AlphaImage> glyphAtlasImage;
    ImageAtlas iconAtlas;

    const MapMode mode;
    
    bool showCollisionBoxes;
    
    enum class FadeState {
        Loaded,
        NeedsFirstPlacement,
        NeedsSecondPlacement,
        CanRemove
    };

    FadeState fadeState = FadeState::Loaded;
public:
    optional<gl::Texture> glyphAtlasTexture;
    optional<gl::Texture> iconAtlasTexture;
};

} // namespace mbgl
