#pragma once

#include <mbgl/gl/state.hpp>
#include <mbgl/gl/value.hpp>

namespace mbgl {
namespace gl {

class Config {
public:
    void resetState() {
        stencilFunc.reset();
        stencilMask.reset();
        stencilTest.reset();
        stencilOp.reset();
        depthRange.reset();
        depthMask.reset();
        depthTest.reset();
        depthFunc.reset();
        blend.reset();
        blendFunc.reset();
        blendColor.reset();
        colorMask.reset();
        clearDepth.reset();
        clearColor.reset();
        clearStencil.reset();
        program.reset();
        lineWidth.reset();
        activeTexture.reset();
        bindFramebuffer.reset();
        viewport.reset();
#ifndef GL_ES_VERSION_2_0
        pixelZoom.reset();
        rasterPos.reset();
#endif // GL_ES_VERSION_2_0
        for (auto& tex : texture) {
            tex.reset();
        }
        vertexBuffer.reset();
        elementBuffer.reset();
        vertexArrayObject.reset();
    }

    void setDirtyState() {
        stencilFunc.setDirty();
        stencilMask.setDirty();
        stencilTest.setDirty();
        stencilOp.setDirty();
        depthRange.setDirty();
        depthMask.setDirty();
        depthTest.setDirty();
        depthFunc.setDirty();
        blend.setDirty();
        blendFunc.setDirty();
        blendColor.setDirty();
        colorMask.setDirty();
        clearDepth.setDirty();
        clearColor.setDirty();
        clearStencil.setDirty();
        program.setDirty();
        lineWidth.setDirty();
        activeTexture.setDirty();
        bindFramebuffer.setDirty();
        viewport.setDirty();
#ifndef GL_ES_VERSION_2_0
        pixelZoom.setDirty();
        rasterPos.setDirty();
#endif // GL_ES_VERSION_2_0
        for (auto& tex : texture) {
            tex.setDirty();
        }
        vertexBuffer.setDirty();
        elementBuffer.setDirty();
        vertexArrayObject.setDirty();
    }

    State<value::StencilFunc> stencilFunc;
    State<value::StencilMask> stencilMask;
    State<value::StencilTest> stencilTest;
    State<value::StencilOp> stencilOp;
    State<value::DepthRange> depthRange;
    State<value::DepthMask> depthMask;
    State<value::DepthTest> depthTest;
    State<value::DepthFunc> depthFunc;
    State<value::Blend> blend;
    State<value::BlendFunc> blendFunc;
    State<value::BlendColor> blendColor;
    State<value::ColorMask> colorMask;
    State<value::ClearDepth> clearDepth;
    State<value::ClearColor> clearColor;
    State<value::ClearStencil> clearStencil;
    State<value::Program> program;
    State<value::LineWidth> lineWidth;
    State<value::ActiveTexture> activeTexture;
    State<value::BindFramebuffer> bindFramebuffer;
    State<value::Viewport> viewport;
#ifndef GL_ES_VERSION_2_0
    State<value::PixelZoom> pixelZoom;
    State<value::RasterPos> rasterPos;
#endif // GL_ES_VERSION_2_0
    std::array<State<value::BindTexture>, 2> texture;
    State<value::BindBuffer<GL_ARRAY_BUFFER>> vertexBuffer;
    State<value::BindBuffer<GL_ELEMENT_ARRAY_BUFFER>> elementBuffer;
    State<value::BindVAO> vertexArrayObject;
};

} // namespace gl
} // namespace mbgl
