#include <mbgl/renderer/paint_parameters.hpp>
#include <mbgl/renderer/update_parameters.hpp>
#include <mbgl/renderer/render_static_data.hpp>
#include <mbgl/map/transform_state.hpp>

namespace mbgl {

PaintParameters::PaintParameters(gl::Context& context_,
                    float pixelRatio_,
                    GLContextMode contextMode_,
                    RendererBackend& backend_,
                    const UpdateParameters& updateParameters,
                    const EvaluatedLight& evaluatedLight_,
                    RenderStaticData& staticData_,
                    ImageManager& imageManager_,
                    LineAtlas& lineAtlas_,
                    Placement::VariableOffsets variableOffsets_)
    : context(context_),
    backend(backend_),
    state(updateParameters.transformState),
    evaluatedLight(evaluatedLight_),
    staticData(staticData_),
    imageManager(imageManager_),
    lineAtlas(lineAtlas_),
    mapMode(updateParameters.mode),
    debugOptions(updateParameters.debugOptions),
    contextMode(contextMode_),
    timePoint(updateParameters.timePoint),
    pixelRatio(pixelRatio_),
    variableOffsets(variableOffsets_),
#ifndef NDEBUG
    programs((debugOptions & MapDebugOptions::Overdraw) ? staticData_.overdrawPrograms : staticData_.programs)
#else
    programs(staticData_.programs)
#endif
{
    // Update the default matrices to the current viewport dimensions.
    state.getProjMatrix(projMatrix);

    // Also compute a projection matrix that aligns with the current pixel grid, taking into account
    // odd viewport sizes.
    state.getProjMatrix(alignedProjMatrix, 1, true);

    // Calculate a second projection matrix with the near plane clipped to 100 so as
    // not to waste lots of depth buffer precision on very close empty space, for layer
    // types (fill-extrusion) that use the depth buffer to emulate real-world space.
    state.getProjMatrix(nearClippedProjMatrix, 100);

    pixelsToGLUnits = {{ 2.0f  / state.getSize().width, -2.0f / state.getSize().height }};

    if (state.getViewportMode() == ViewportMode::FlippedY) {
        pixelsToGLUnits[1] *= -1;
    }
}

mat4 PaintParameters::matrixForTile(const UnwrappedTileID& tileID, bool aligned) const {
    mat4 matrix;
    state.matrixFor(matrix, tileID);
    matrix::multiply(matrix, aligned ? alignedProjMatrix : projMatrix, matrix);
    return matrix;
}

gfx::DepthMode PaintParameters::depthModeForSublayer(uint8_t n, gfx::DepthMaskType mask) const {
    float nearDepth = ((1 + currentLayer) * numSublayers + n) * depthEpsilon;
    float farDepth = nearDepth + depthRangeSize;
    return gfx::DepthMode { gfx::DepthFunctionType::LessEqual, mask, { nearDepth, farDepth } };
}

gfx::DepthMode PaintParameters::depthModeFor3D(gfx::DepthMaskType mask) const {
    return gfx::DepthMode { gfx::DepthFunctionType::LessEqual, mask, { 0.0, 1.0 } };
}

gfx::StencilMode PaintParameters::stencilModeForClipping(const ClipID& id) const {
    return gfx::StencilMode {
        gfx::StencilMode::Equal { static_cast<uint32_t>(id.mask.to_ulong()) },
        static_cast<int32_t>(id.reference.to_ulong()),
        0,
        gfx::StencilOpType::Keep,
        gfx::StencilOpType::Keep,
        gfx::StencilOpType::Replace
    };
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
