#pragma once

#include <mbgl/tile/tile.hpp>
#include <mbgl/tile/geometry_tile_worker.hpp>
#include <mbgl/renderer/image_manager.hpp>
#include <mbgl/text/glyph_manager.hpp>
#include <mbgl/text/placement_config.hpp>
#include <mbgl/text/collision_tile.hpp>
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

    void setPlacementConfig(const PlacementConfig&) override;
    void setLayers(const std::vector<Immutable<style::Layer::Impl>>&) override;
    
    void onGlyphsAvailable(GlyphMap) override;
    void onImagesAvailable(ImageMap) override;
    
    void getGlyphs(GlyphDependencies);
    void getImages(ImageDependencies);

    void upload(gl::Context&) override;
    Bucket* getBucket(const style::Layer::Impl&) const override;

    Size bindGlyphAtlas(gl::Context&);
    Size bindIconAtlas(gl::Context&);

    void queryRenderedFeatures(
            std::unordered_map<std::string, std::vector<Feature>>& result,
            const GeometryCoordinates& queryGeometry,
            const TransformState&,
            const std::vector<const RenderLayer*>& layers,
            const RenderedQueryOptions& options) override;

    void querySourceFeatures(
        std::vector<Feature>& result,
        const SourceQueryOptions&) override;

    void cancel() override;

    class LayoutResult {
    public:
        std::unordered_map<std::string, std::shared_ptr<Bucket>> nonSymbolBuckets;
        std::unique_ptr<FeatureIndex> featureIndex;
        std::unique_ptr<GeometryTileData> tileData;
        uint64_t correlationID;

        LayoutResult(std::unordered_map<std::string, std::shared_ptr<Bucket>> nonSymbolBuckets_,
                     std::unique_ptr<FeatureIndex> featureIndex_,
                     std::unique_ptr<GeometryTileData> tileData_,
                     uint64_t correlationID_)
            : nonSymbolBuckets(std::move(nonSymbolBuckets_)),
              featureIndex(std::move(featureIndex_)),
              tileData(std::move(tileData_)),
              correlationID(correlationID_) {}
    };
    void onLayout(LayoutResult);

    class PlacementResult {
    public:
        std::unordered_map<std::string, std::shared_ptr<Bucket>> symbolBuckets;
        std::unique_ptr<CollisionTile> collisionTile;
        optional<AlphaImage> glyphAtlasImage;
        optional<PremultipliedImage> iconAtlasImage;
        uint64_t correlationID;

        PlacementResult(std::unordered_map<std::string, std::shared_ptr<Bucket>> symbolBuckets_,
                        std::unique_ptr<CollisionTile> collisionTile_,
                        optional<AlphaImage> glyphAtlasImage_,
                        optional<PremultipliedImage> iconAtlasImage_,
                        uint64_t correlationID_)
            : symbolBuckets(std::move(symbolBuckets_)),
              collisionTile(std::move(collisionTile_)),
              glyphAtlasImage(std::move(glyphAtlasImage_)),
              iconAtlasImage(std::move(iconAtlasImage_)),
              correlationID(correlationID_) {}
    };
    void onPlacement(PlacementResult);

    void onError(std::exception_ptr);
    
    float yStretch() const override;
    
protected:
    const GeometryTileData* getData() {
        return data.get();
    }

private:
    void markObsolete();
    void invokePlacement();

    const std::string sourceID;

    // Used to signal the worker that it should abandon parsing this tile as soon as possible.
    std::atomic<bool> obsolete { false };

    std::shared_ptr<Mailbox> mailbox;
    Actor<GeometryTileWorker> worker;

    GlyphManager& glyphManager;
    ImageManager& imageManager;

    uint64_t correlationID = 0;
    optional<PlacementConfig> requestedConfig;

    std::unordered_map<std::string, std::shared_ptr<Bucket>> nonSymbolBuckets;
    std::unique_ptr<FeatureIndex> featureIndex;
    std::unique_ptr<const GeometryTileData> data;

    optional<AlphaImage> glyphAtlasImage;
    optional<PremultipliedImage> iconAtlasImage;

    std::unordered_map<std::string, std::shared_ptr<Bucket>> symbolBuckets;
    std::unique_ptr<CollisionTile> collisionTile;
    
    util::Throttler placementThrottler;
    float lastYStretch;

public:
    optional<gl::Texture> glyphAtlasTexture;
    optional<gl::Texture> iconAtlasTexture;
};

} // namespace mbgl
