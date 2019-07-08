#pragma once
#include <mbgl/layout/layout.hpp>
#include <mbgl/renderer/render_pass.hpp>
#include <mbgl/renderer/render_tree.hpp>
#include <mbgl/style/layer_properties.hpp>
#include <mbgl/tile/geometry_tile_data.hpp>
#include <mbgl/util/mat4.hpp>

#include <functional>
#include <memory>
#include <string>

namespace mbgl {

class Bucket;
class TransitionParameters;
class PropertyEvaluationParameters;
class UploadParameters;
class PaintParameters;
class RenderSource;
class RenderTile;
class TransformState;
class PatternAtlas;
class LineAtlas;

class LayerRenderData {
public:
    std::shared_ptr<Bucket> bucket;
    Immutable<style::LayerProperties> layerProperties;
};

class LayerPlacementData {
public:
    std::reference_wrapper<Bucket> bucket;
    std::reference_wrapper<const RenderTile> tile;
    std::shared_ptr<FeatureIndex> featureIndex;
};

class LayerPrepareParameters {
public:
    RenderSource* source;
    ImageManager& imageManager;
    PatternAtlas& patternAtlas;
    LineAtlas& lineAtlas;
    const TransformState& state;
};

class LayerRenderItem;
using RenderTiles = std::vector<std::reference_wrapper<const RenderTile>>;
using LayerUploader = std::function<void(gfx::UploadPass&)>;
using LayerRenderer = std::function<void(PaintParameters&, const LayerRenderItem&)>;

class LayerRenderItem final : public RenderItem {
public:
    LayerRenderItem(RenderTiles, LayerRenderer, LayerUploader, Immutable<style::LayerProperties>, RenderPass);
    ~LayerRenderItem();

    RenderTiles renderTiles;
    LayerRenderer renderer;
    LayerUploader uploader;
    Immutable<style::LayerProperties> evaluatedProperties;

    void checkRenderability(const PaintParameters&, uint32_t activeBindingCount) const;
private:
    bool hasRenderPass(RenderPass pass) const override { return bool(renderPass & pass); }
    void upload(gfx::UploadPass& pass) const override { if (uploader) uploader(pass);}
    void render(PaintParameters& parameters) const override {
        assert(renderer);
        renderer(parameters, *this);
    }
    const std::string& getName() const override { return evaluatedProperties->baseImpl->id; } 
    RenderPass renderPass;
    // Some layers may not render correctly on some hardware when the vertex attribute limit of
    // that GPU is exceeded. More attributes are used when adding many data driven paint properties
    // to a layer.
    mutable bool hasRenderFailures = false;
};

class RenderLayer {
protected:
    RenderLayer(Immutable<style::LayerProperties>);

public:
    virtual ~RenderLayer() = default;

    LayerRenderItem createRenderItem();
    // Begin transitions for any properties that have changed since the last frame.
    virtual void transition(const TransitionParameters&) = 0;

    // Overloaded version for transitions to a new layer impl.
    void transition(const TransitionParameters&, Immutable<style::Layer::Impl> newImpl);

    // Fully evaluate possibly-transitioning paint properties based on a zoom level.
    // Updates the contained `evaluatedProperties` member.
    virtual void evaluate(const PropertyEvaluationParameters&) = 0;

    // Returns true if any paint properties have active transitions.
    virtual bool hasTransition() const = 0;

    // Returns true if the layer has a pattern property and is actively crossfading.
    virtual bool hasCrossfade() const = 0;

    // Returns true is the layer is subject to placement.
    bool needsPlacement() const;

    const std::string& getID() const;

    // Checks whether this layer can be rendered.
    bool needsRendering() const;

    // Checks whether the given zoom is inside this layer zoom range.
    bool supportsZoom(float zoom) const;

    // Check wether the given geometry intersects
    // with the feature
    virtual bool queryIntersectsFeature(
            const GeometryCoordinates&,
            const GeometryTileFeature&,
            const float,
            const TransformState&,
            const float,
            const mat4&) const { return false; };

    virtual void prepare(const LayerPrepareParameters&);

    const std::vector<LayerPlacementData>& getPlacementData() const { 
        return placementData; 
    }

    // Latest evaluated properties.
    Immutable<style::LayerProperties> evaluatedProperties;
    // Private implementation
    Immutable<style::Layer::Impl> baseImpl;

    virtual void markContextDestroyed();

    // TODO: Only for background layers.
    virtual optional<Color> getSolidBackground() const;

protected:
    virtual LayerRenderer createRenderer() = 0;
    virtual LayerUploader createUploader();

    // Stores current set of tiles to be rendered for this layer.
    RenderTiles renderTiles;

    // Stores what render passes this layer is currently enabled for. This depends on the
    // evaluated StyleProperties object and is updated accordingly.
    RenderPass passes = RenderPass::None;

    std::vector<LayerPlacementData> placementData;

private:
    RenderTiles filterRenderTiles(RenderTiles) const;
};

} // namespace mbgl
