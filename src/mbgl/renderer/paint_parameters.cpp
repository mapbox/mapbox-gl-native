#include <mbgl/renderer/paint_parameters.hpp>
#include <mbgl/renderer/update_parameters.hpp>
#include <mbgl/renderer/render_static_data.hpp>
#include <mbgl/renderer/render_source.hpp>
#include <mbgl/renderer/render_tile.hpp>
#include <mbgl/gfx/command_encoder.hpp>
#include <mbgl/gfx/render_pass.hpp>
#include <mbgl/gfx/cull_face_mode.hpp>
#include <mbgl/map/transform_state.hpp>

namespace mbgl {

TransformParameters::TransformParameters(const TransformState& state_)
    : state(state_) {
    // Update the default matrices to the current viewport dimensions.
    state.getProjMatrix(projMatrix);

    // Also compute a projection matrix that aligns with the current pixel grid, taking into account
    // odd viewport sizes.
    state.getProjMatrix(alignedProjMatrix, 1, true);

    // Calculate a second projection matrix with the near plane moved further,
    // to a tenth of the far value, so as not to waste depth buffer precision on
    // very close empty space, for layer types (fill-extrusion) that use the
    // depth buffer to emulate real-world space.
    state.getProjMatrix(nearClippedProjMatrix, 0.1 * state.getCameraToCenterDistance());
}

PaintParameters::PaintParameters(gfx::Context& context_,
                    float pixelRatio_,
                    gfx::RendererBackend& backend_,
                    const EvaluatedLight& evaluatedLight_,
                    MapMode mode_,
                    MapDebugOptions debugOptions_,
                    TimePoint timePoint_,
                    const TransformParameters& transformParams_,
                    RenderStaticData& staticData_,
                    LineAtlas& lineAtlas_,
                    PatternAtlas& patternAtlas_)
    : context(context_),
    backend(backend_),
    encoder(context.createCommandEncoder()),
    transformParams(transformParams_),
    state(transformParams_.state),
    evaluatedLight(evaluatedLight_),
    staticData(staticData_),
    lineAtlas(lineAtlas_),
    patternAtlas(patternAtlas_),
    mapMode(mode_),
    debugOptions(debugOptions_),
    timePoint(timePoint_),
    pixelRatio(pixelRatio_),
#ifndef NDEBUG
    programs((debugOptions & MapDebugOptions::Overdraw) ? staticData_.overdrawPrograms : staticData_.programs)
#else
    programs(staticData_.programs)
#endif
{
    pixelsToGLUnits = {{ 2.0f  / state.getSize().width, -2.0f / state.getSize().height }};

    if (state.getViewportMode() == ViewportMode::FlippedY) {
        pixelsToGLUnits[1] *= -1;
    }
}

PaintParameters::~PaintParameters() = default;

mat4 PaintParameters::matrixForTile(const UnwrappedTileID& tileID, bool aligned) const {
    mat4 matrix;
    state.matrixFor(matrix, tileID);
    matrix::multiply(matrix, aligned ? transformParams.alignedProjMatrix : transformParams.projMatrix, matrix);
    return matrix;
}

gfx::DepthMode PaintParameters::depthModeForSublayer(uint8_t n, gfx::DepthMaskType mask) const {
    if (currentLayer < opaquePassCutoff) {
        return gfx::DepthMode::disabled();
    }
    float depth = depthRangeSize + ((1 + currentLayer) * numSublayers + n) * depthEpsilon;
    return gfx::DepthMode { gfx::DepthFunctionType::LessEqual, mask, { depth, depth } };
}

gfx::DepthMode PaintParameters::depthModeFor3D() const {
    return gfx::DepthMode{ gfx::DepthFunctionType::LessEqual,
                           gfx::DepthMaskType::ReadWrite,
                           { 0.0, depthRangeSize } };
}

void PaintParameters::clearStencil() {
    nextStencilID = 1;
    context.clearStencilBuffer(0b00000000);
}

namespace {

// Detects a difference in keys of renderTiles and tileClippingMaskIDs
bool tileIDsIdentical(const RenderTiles& renderTiles,
                      const std::map<UnwrappedTileID, int32_t>& tileClippingMaskIDs) {
    assert(renderTiles);
    assert(std::is_sorted(renderTiles->begin(), renderTiles->end(),
                          [](const RenderTile& a, const RenderTile& b) { return a.id < b.id; }));
    if (renderTiles->size() != tileClippingMaskIDs.size()) {
        return false;
    }
    return std::equal(renderTiles->begin(), renderTiles->end(), tileClippingMaskIDs.begin(),
                      [](const RenderTile& a, const auto& b) { return a.id == b.first; });
}

} // namespace

void PaintParameters::renderTileClippingMasks(const RenderTiles& renderTiles) {
    if (!renderTiles || renderTiles->empty() || tileIDsIdentical(renderTiles, tileClippingMaskIDs)) {
        // The current stencil mask is for this source already; no need to draw another one.
        return;
    }

    if (nextStencilID + renderTiles->size() > 256) {
        // we'll run out of fresh IDs so we need to clear and start from scratch
        clearStencil();
    }

    tileClippingMaskIDs.clear();

    auto& program = staticData.programs.clippingMask;
    const style::Properties<>::PossiblyEvaluated properties {};
    const ClippingMaskProgram::Binders paintAttributeData(properties, 0);

    for (const RenderTile& renderTile : *renderTiles) {
        const int32_t stencilID = nextStencilID++;
        tileClippingMaskIDs.emplace(renderTile.id, stencilID);

        program.draw(
            context,
            *renderPass,
            gfx::Triangles(),
            gfx::DepthMode::disabled(),
            gfx::StencilMode {
                gfx::StencilMode::Always{},
                stencilID,
                0b11111111,
                gfx::StencilOpType::Keep,
                gfx::StencilOpType::Keep,
                gfx::StencilOpType::Replace
            },
            gfx::ColorMode::disabled(),
            gfx::CullFaceMode::disabled(),
            *staticData.quadTriangleIndexBuffer,
            staticData.tileTriangleSegments,
            program.computeAllUniformValues(
                ClippingMaskProgram::LayoutUniformValues {
                    uniforms::matrix::Value( matrixForTile(renderTile.id) ),
                },
                paintAttributeData,
                properties,
                state.getZoom()
            ),
            program.computeAllAttributeBindings(
                *staticData.tileVertexBuffer,
                paintAttributeData,
                properties
            ),
            ClippingMaskProgram::TextureBindings{},
            "clipping/" + util::toString(stencilID)
        );
    }
}

gfx::StencilMode PaintParameters::stencilModeForClipping(const UnwrappedTileID& tileID) const {
    auto it = tileClippingMaskIDs.find(tileID);
    assert(it != tileClippingMaskIDs.end());
    const int32_t id = it != tileClippingMaskIDs.end() ? it->second : 0b00000000;
    return gfx::StencilMode{ gfx::StencilMode::Equal{ 0b11111111 },
                             id,
                             0b00000000,
                             gfx::StencilOpType::Keep,
                             gfx::StencilOpType::Keep,
                             gfx::StencilOpType::Replace };
}

gfx::StencilMode PaintParameters::stencilModeFor3D() {
    if (nextStencilID + 1 > 256) {
        clearStencil();
    }

    // We're potentially destroying the stencil clipping mask in this pass. That means we'll have
    // to recreate it for the next source if any.
    tileClippingMaskIDs.clear();

    const int32_t id = nextStencilID++;
    return gfx::StencilMode{ gfx::StencilMode::NotEqual{ 0b11111111 },
                             id,
                             0b11111111,
                             gfx::StencilOpType::Keep,
                             gfx::StencilOpType::Keep,
                             gfx::StencilOpType::Replace };
}

gfx::ColorMode PaintParameters::colorModeForRenderPass() const {
    if (debugOptions & MapDebugOptions::Overdraw) {
        const float overdraw = 1.0f / 8.0f;
        return gfx::ColorMode {
            gfx::ColorMode::Add {
                gfx::ColorBlendFactorType::ConstantColor,
                gfx::ColorBlendFactorType::One
            },
            Color { overdraw, overdraw, overdraw, 0.0f },
            gfx::ColorMode::Mask { true, true, true, true }
        };
    } else if (pass == RenderPass::Translucent) {
        return gfx::ColorMode::alphaBlended();
    } else {
        return gfx::ColorMode::unblended();
    }
}

} // namespace mbgl
