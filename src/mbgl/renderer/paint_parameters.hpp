#pragma once

#include <mbgl/renderer/render_pass.hpp>
#include <mbgl/renderer/render_light.hpp>
#include <mbgl/renderer/mode.hpp>
#include <mbgl/map/mode.hpp>
#include <mbgl/gfx/depth_mode.hpp>
#include <mbgl/gfx/stencil_mode.hpp>
#include <mbgl/gfx/color_mode.hpp>
#include <mbgl/util/mat4.hpp>
#include <mbgl/algorithm/generate_clip_ids.hpp>
#include <mbgl/text/placement.hpp>

#include <array>

namespace mbgl {

class RendererBackend;
class UpdateParameters;
class RenderStaticData;
class Programs;
class TransformState;
class ImageManager;
class LineAtlas;
class UnwrappedTileID;

namespace gl {
class Context;
} // namespace gl

class PaintParameters {
public:
    PaintParameters(gl::Context&,
                    float pixelRatio,
                    GLContextMode,
                    RendererBackend&,
                    const UpdateParameters&,
                    const EvaluatedLight&,
                    RenderStaticData&,
                    ImageManager&,
                    LineAtlas&,
                    Placement::VariableOffsets);

    gl::Context& context;
    RendererBackend& backend;

    const TransformState& state;
    const EvaluatedLight& evaluatedLight;

    RenderStaticData& staticData;
    ImageManager& imageManager;
    LineAtlas& lineAtlas;

    RenderPass pass = RenderPass::Opaque;
    MapMode mapMode;
    MapDebugOptions debugOptions;
    GLContextMode contextMode;
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
