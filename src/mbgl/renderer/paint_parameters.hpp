#pragma once

#include <mbgl/renderer/render_pass.hpp>
#include <mbgl/renderer/render_light.hpp>
#include <mbgl/map/mode.hpp>
#include <mbgl/gl/depth_mode.hpp>
#include <mbgl/gl/stencil_mode.hpp>
#include <mbgl/gl/color_mode.hpp>
#include <mbgl/util/mat4.hpp>
#include <mbgl/algorithm/generate_clip_ids.hpp>

#include <array>

namespace mbgl {

class RendererBackend;
class UpdateParameters;
class RenderStaticData;
class FrameHistory;
class Programs;
class TransformState;
class ImageManager;
class LineAtlas;
class UnwrappedTileID;

class PaintParameters {
public:
    PaintParameters(gl::Context&,
                    float pixelRatio,
                    GLContextMode,
                    RendererBackend&,
                    const UpdateParameters&,
                    const EvaluatedLight&,
                    RenderStaticData&,
                    FrameHistory&,
                    ImageManager&,
                    LineAtlas&);

    gl::Context& context;
    RendererBackend& backend;

    const TransformState& state;
    const EvaluatedLight& evaluatedLight;

    RenderStaticData& staticData;
    FrameHistory& frameHistory;
    ImageManager& imageManager;
    LineAtlas& lineAtlas;

    RenderPass pass = RenderPass::Opaque;
    MapMode mapMode;
    MapDebugOptions debugOptions;
    GLContextMode contextMode;
    TimePoint timePoint;

    float pixelRatio;
    std::array<float, 2> pixelsToGLUnits;
    algorithm::ClipIDGenerator clipIDGenerator;

    Programs& programs;

    gl::DepthMode depthModeForSublayer(uint8_t n, gl::DepthMode::Mask) const;
    gl::StencilMode stencilModeForClipping(const ClipID&) const;
    gl::ColorMode colorModeForRenderPass() const;

    mat4 matrixForTile(const UnwrappedTileID&);

    mat4 projMatrix;
    mat4 nearClippedProjMatrix;

    int numSublayers = 3;
    uint32_t currentLayer;
    float depthRangeSize;
    const float depthEpsilon = 1.0f / (1 << 16);
};

} // namespace mbgl
