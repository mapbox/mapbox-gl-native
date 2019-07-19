#pragma once

#include <mbgl/renderer/render_pass.hpp>
#include <mbgl/renderer/render_light.hpp>
#include <mbgl/renderer/render_source.hpp>
#include <mbgl/map/mode.hpp>
#include <mbgl/map/transform_state.hpp>
#include <mbgl/gfx/depth_mode.hpp>
#include <mbgl/gfx/stencil_mode.hpp>
#include <mbgl/gfx/color_mode.hpp>
#include <mbgl/util/mat4.hpp>

#include <array>
#include <map>
#include <vector>

namespace mbgl {

class UpdateParameters;
class RenderStaticData;
class Programs;
class TransformState;
class ImageManager;
class LineAtlas;
class PatternAtlas;
class UnwrappedTileID;

namespace gfx {
class Context;
class RendererBackend;
class CommandEncoder;
class RenderPass;
} // namespace gfx


class TransformParameters {
public:
    TransformParameters(const TransformState&);
    mat4 projMatrix;
    mat4 alignedProjMatrix;
    mat4 nearClippedProjMatrix;
    const TransformState state;
};

class PaintParameters {
public:
    PaintParameters(gfx::Context&,
                    float pixelRatio,
                    gfx::RendererBackend&,
                    const EvaluatedLight&,
                    MapMode,
                    MapDebugOptions,
                    TimePoint,
                    const TransformParameters&,
                    RenderStaticData&,
                    LineAtlas&,
                    PatternAtlas&);
    ~PaintParameters();

    gfx::Context& context;
    gfx::RendererBackend& backend;
    std::unique_ptr<gfx::CommandEncoder> encoder;
    std::unique_ptr<gfx::RenderPass> renderPass;

    const TransformParameters& transformParams;
    const TransformState& state;
    const EvaluatedLight& evaluatedLight;

    RenderStaticData& staticData;
    LineAtlas& lineAtlas;
    PatternAtlas& patternAtlas;

    RenderPass pass = RenderPass::Opaque;
    MapMode mapMode;
    MapDebugOptions debugOptions;
    TimePoint timePoint;

    float pixelRatio;
    std::array<float, 2> pixelsToGLUnits;

    Programs& programs;

    gfx::DepthMode depthModeForSublayer(uint8_t n, gfx::DepthMaskType) const;
    gfx::DepthMode depthModeFor3D() const;
    gfx::ColorMode colorModeForRenderPass() const;

    mat4 matrixForTile(const UnwrappedTileID&, bool aligned = false) const;

    // Stencil handling
public:
    void renderTileClippingMasks(const RenderTiles&);
    gfx::StencilMode stencilModeForClipping(const UnwrappedTileID&) const;
    gfx::StencilMode stencilModeFor3D();

private:
    void clearStencil();

    // This needs to be an ordered map so that we have the same order as the renderTiles.
    std::map<UnwrappedTileID, int32_t> tileClippingMaskIDs;
    int32_t nextStencilID = 1;

public:
    int numSublayers = 3;
    uint32_t currentLayer;
    float depthRangeSize;
    const float depthEpsilon = 1.0f / (1 << 16);
    uint32_t opaquePassCutoff = 0;
    float symbolFadeChange;
};

} // namespace mbgl
