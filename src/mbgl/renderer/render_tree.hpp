#pragma once

#include <mbgl/renderer/paint_parameters.hpp>

#include <cassert>
#include <memory>
#include <string>
#include <vector>

namespace mbgl {

class PaintParameters;
class PatternAtlas;

namespace gfx {
class UploadPass;
} // namespace gfx

class RenderItem {
public:
    virtual ~RenderItem() = default;
    virtual void upload(gfx::UploadPass&) const = 0;
    virtual void render(PaintParameters&) const = 0;
    virtual bool hasRenderPass(RenderPass) const = 0;
    virtual const std::string& getName() const = 0; 
};

using RenderItems = std::vector<std::reference_wrapper<const RenderItem>>;

class RenderTreeParameters {
public:
    explicit RenderTreeParameters(
        const TransformState& state_,
        MapMode mapMode_,
        MapDebugOptions debugOptions_,
        TimePoint timePoint_,
        const EvaluatedLight& light_)
        : transformParams(state_),
          mapMode(mapMode_),
          debugOptions(debugOptions_),
          timePoint(timePoint_),
          light(light_) {
    }
    TransformParameters transformParams;
    MapMode mapMode;
    MapDebugOptions debugOptions;
    TimePoint timePoint;
    EvaluatedLight light;
    bool has3D = false;
    uint32_t opaquePassCutOff = 0;
    Color backgroundColor;
    float symbolFadeChange = 0.0f;
    bool needsRepaint = false;
    bool loaded = false;
};

class RenderTree {
public:
    virtual ~RenderTree() = default;
    // Render items
    virtual RenderItems getLayerRenderItems() const = 0;
    virtual RenderItems getSourceRenderItems() const = 0;
    // Resources
    virtual LineAtlas& getLineAtlas() const = 0;
    virtual PatternAtlas& getPatternAtlas() const = 0;
    // Parameters
    const RenderTreeParameters& getParameters() const {
        return *parameters;
    }
protected:
    RenderTree(std::unique_ptr<RenderTreeParameters> parameters_)
        : parameters(std::move(parameters_)) {
        assert(parameters);
    }
    std::unique_ptr<RenderTreeParameters> parameters;
};


} // namespace mbgl
