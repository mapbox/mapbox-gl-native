#pragma once

#include <mbgl/renderer/render_pass.hpp>
#include <mbgl/renderer/render_light.hpp>
#include <mbgl/map/mode.hpp>
#include <mbgl/gfx/depth_mode.hpp>
#include <mbgl/gfx/stencil_mode.hpp>
#include <mbgl/gfx/color_mode.hpp>
#include <mbgl/util/mat4.hpp>
#include <mbgl/algorithm/generate_clip_ids.hpp>
#include <mbgl/text/placement.hpp>

#include <array>

namespace mbgl {

class UpdateParameters;
class RenderStaticData;
class Programs;
class TransformState;
class ImageManager;
class LineAtlas;
class UnwrappedTileID;

namespace gfx {
class Context;
class RendererBackend;
class CommandEncoder;
class RenderPass;
} // namespace gfx

class PaintParameters {
public:
    PaintParameters(gfx::Context&,
                    float pixelRatio,
                    gfx::RendererBackend&,
                    const UpdateParameters&,
                    const EvaluatedLight&,
                    RenderStaticData&,
                    ImageManager&,
                    LineAtlas&,
                    Placement::VariableOffsets);
    ~PaintParameters();

    gfx::Context& context;
    gfx::RendererBackend& backend;
    const std::unique_ptr<gfx::CommandEncoder> encoder;
    std::unique_ptr<gfx::RenderPass> renderPass;

    const TransformState& state;
    const EvaluatedLight& evaluatedLight;

    RenderStaticData& staticData;
    ImageManager& imageManager;
    LineAtlas& lineAtlas;

    RenderPass pass = RenderPass::Opaque;
    MapMode mapMode;
    MapDebugOptions debugOptions;
    TimePoint timePoint;

    float pixelRatio;
    Placement::VariableOffsets variableOffsets;
    std::array<float, 2> pixelsToGLUnits;
    algorithm::ClipIDGenerator clipIDGenerator;

    Programs& programs;

    gfx::DepthMode depthModeForSublayer(uint8_t n, gfx::DepthMaskType) const;
    gfx::DepthMode depthModeFor3D(gfx::DepthMaskType) const;
    gfx::StencilMode stencilModeForClipping(const ClipID&) const;
    gfx::ColorMode colorModeForRenderPass() const;

    mat4 matrixForTile(const UnwrappedTileID&, bool aligned = false) const;

    mat4 projMatrix;
    mat4 alignedProjMatrix;
    mat4 nearClippedProjMatrix;

    int numSublayers = 3;
    uint32_t currentLayer;
    float depthRangeSize;
    const float depthEpsilon = 1.0f / (1 << 16);
    
    float symbolFadeChange;
};

} // namespace mbgl
