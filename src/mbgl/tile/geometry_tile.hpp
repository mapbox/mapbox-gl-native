#pragma once

#include <mbgl/actor/actor.hpp>
#include <mbgl/geometry/feature_index.hpp>
#include <mbgl/gfx/texture.hpp>
#include <mbgl/renderer/image_manager.hpp>
#include <mbgl/text/glyph_manager.hpp>
#include <mbgl/tile/tile.hpp>
#include <mbgl/tile/geometry_tile_worker.hpp>
#include <mbgl/util/feature.hpp>
#include <mbgl/util/optional.hpp>

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
class TileAtlasTextures;

class GeometryTile : public Tile, public GlyphRequestor, public ImageRequestor {
public:
    GeometryTile(const OverscaledTileID&,
                 std::string sourceID,
                 const TileParameters&);

    ~GeometryTile() override;

    void setError(std::exception_ptr);
    void setData(std::unique_ptr<const GeometryTileData>);
    // Resets the tile's data and layers and leaves the tile in pending state, waiting for the new
    // data and layers to come.
    void reset();

    std::unique_ptr<TileRenderData> createRenderData() override;
    void setLayers(const std::vector<Immutable<style::LayerProperties>>&) override;
    void setShowCollisionBoxes(const bool showCollisionBoxes) override;

    void onGlyphsAvailable(GlyphMap) override;
    void onImagesAvailable(ImageMap, ImageMap, ImageVersionMap versionMap, uint64_t imageCorrelationID) override;
    
    void getGlyphs(GlyphDependencies);
    void getImages(ImageRequestPair);

    bool layerPropertiesUpdated(const Immutable<style::LayerProperties>&) override;

    void queryRenderedFeatures(std::unordered_map<std::string, std::vector<Feature>>& result,
                               const GeometryCoordinates& queryGeometry, const TransformState&,
                               const std::unordered_map<std::string, const RenderLayer*>& layers,
                               const RenderedQueryOptions& options, const mat4& projMatrix,
                               const SourceFeatureState& featureState) override;

    void querySourceFeatures(
        std::vector<Feature>& result,
        const SourceQueryOptions&) override;

    float getQueryPadding(const std::unordered_map<std::string, const RenderLayer*>&) override;

    void cancel() override;

    class LayoutResult {
    public:
        std::unordered_map<std::string, LayerRenderData> layerRenderData;
        std::shared_ptr<FeatureIndex> featureIndex;
        optional<AlphaImage> glyphAtlasImage;
        ImageAtlas iconAtlas;

        LayerRenderData* getLayerRenderData(const style::Layer::Impl&);

        LayoutResult(std::unordered_map<std::string, LayerRenderData> renderData_,
                     std::unique_ptr<FeatureIndex> featureIndex_,
                     optional<AlphaImage> glyphAtlasImage_,
                     ImageAtlas iconAtlas_)
            : layerRenderData(std::move(renderData_)),
              featureIndex(std::move(featureIndex_)),
              glyphAtlasImage(std::move(glyphAtlasImage_)),
              iconAtlas(std::move(iconAtlas_)) {}
    };
    void onLayout(std::shared_ptr<LayoutResult>, uint64_t correlationID);

    void onError(std::exception_ptr, uint64_t correlationID);

    bool holdForFade() const override;
    void markRenderedIdeal() override;
    void markRenderedPreviously() override;
    void performedFadePlacement() override;
    const std::shared_ptr<FeatureIndex> getFeatureIndex() const;
    
    const std::string sourceID;

    void setFeatureState(const LayerFeatureStates&) override;

protected:
    const GeometryTileData* getData() const;
    LayerRenderData* getLayerRenderData(const style::Layer::Impl&);

private:
    void markObsolete();

    // Used to signal the worker that it should abandon parsing this tile as soon as possible.
    std::atomic<bool> obsolete { false };

    std::shared_ptr<Mailbox> mailbox;
    Actor<GeometryTileWorker> worker;

    std::shared_ptr<FileSource> fileSource;
    GlyphManager& glyphManager;
    ImageManager& imageManager;

    uint64_t correlationID = 0;

    std::shared_ptr<LayoutResult> layoutResult;
    std::shared_ptr<TileAtlasTextures> atlasTextures;

    const MapMode mode;
    
    bool showCollisionBoxes;
    
    enum class FadeState {
        Loaded,
        NeedsFirstPlacement,
        NeedsSecondPlacement,
        CanRemove
    };

    FadeState fadeState = FadeState::Loaded;
};

} // namespace mbgl
