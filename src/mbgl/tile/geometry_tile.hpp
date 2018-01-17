#pragma once

#include <mbgl/tile/tile.hpp>
#include <mbgl/tile/geometry_tile_worker.hpp>
#include <mbgl/renderer/image_manager.hpp>
#include <mbgl/text/glyph_manager.hpp>
#include <mbgl/util/feature.hpp>
#include <mbgl/util/throttler.hpp>
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
    void onImagesAvailable(ImageMap, uint64_t imageCorrelationID) override;
    
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
            const CollisionIndex& collisionIndex) override;

    void querySourceFeatures(
        std::vector<Feature>& result,
        const SourceQueryOptions&) override;

    void cancel() override;

    class LayoutResult {
    public:
        std::unordered_map<std::string, std::shared_ptr<Bucket>> nonSymbolBuckets;
        std::unique_ptr<FeatureIndex> featureIndex;
        std::unique_ptr<GeometryTileData> tileData;

        LayoutResult(std::unordered_map<std::string, std::shared_ptr<Bucket>> nonSymbolBuckets_,
                     std::unique_ptr<FeatureIndex> featureIndex_,
                     std::unique_ptr<GeometryTileData> tileData_)
            : nonSymbolBuckets(std::move(nonSymbolBuckets_)),
              featureIndex(std::move(featureIndex_)),
              tileData(std::move(tileData_)) {}
    };
    void onLayout(LayoutResult, uint64_t correlationID);

    class PlacementResult {
    public:
        std::unordered_map<std::string, std::shared_ptr<Bucket>> symbolBuckets;
        optional<AlphaImage> glyphAtlasImage;
        optional<PremultipliedImage> iconAtlasImage;

        PlacementResult(std::unordered_map<std::string, std::shared_ptr<Bucket>> symbolBuckets_,
                        optional<AlphaImage> glyphAtlasImage_,
                        optional<PremultipliedImage> iconAtlasImage_)
            : symbolBuckets(std::move(symbolBuckets_)),
              glyphAtlasImage(std::move(glyphAtlasImage_)),
              iconAtlasImage(std::move(iconAtlasImage_)) {}
    };
    void onPlacement(PlacementResult, uint64_t correlationID);

    void onError(std::exception_ptr, uint64_t correlationID);
    
    bool holdForFade() const override;
    void markRenderedIdeal() override;
    void markRenderedPreviously() override;
    void performedFadePlacement() override;
    
    void commitFeatureIndex() override;
    
protected:
    const GeometryTileData* getData() {
        return data.get();
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

    std::unordered_map<std::string, std::shared_ptr<Bucket>> nonSymbolBuckets;
    std::unique_ptr<FeatureIndex> featureIndex;
    std::unique_ptr<FeatureIndex> pendingFeatureIndex;
    std::unique_ptr<const GeometryTileData> data;
    std::unique_ptr<const GeometryTileData> pendingData;

    optional<AlphaImage> glyphAtlasImage;
    optional<PremultipliedImage> iconAtlasImage;

    std::unordered_map<std::string, std::shared_ptr<Bucket>> symbolBuckets;

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
