#ifndef MBGL_GL_GL_CONFIG
#define MBGL_GL_GL_CONFIG

#include <cstdint>
#include <tuple>
#include <array>

#include <mbgl/gl/gl_values.hpp>

namespace mbgl {
namespace gl {

template <typename T>
class Value {
public:
    inline void operator=(const typename T::Type& value) {
        if (dirty || current != value) {
            dirty = false;
            current = value;
            T::Set(current);
        }
    }

    inline void reset() {
        dirty = true;
        current = T::Default;
        T::Set(current);
    }

    inline void setDirty() {
        dirty = true;
    }

private:
    typename T::Type current = T::Default;
    bool dirty = false;
};

class Config {
public:
    void reset() {
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
        colorMask.reset();
        clearDepth.reset();
        clearColor.reset();
        clearStencil.reset();
        program.reset();
        lineWidth.reset();
    }

    void setDirty() {
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
        colorMask.setDirty();
        clearDepth.setDirty();
        clearColor.setDirty();
        clearStencil.setDirty();
        program.setDirty();
        lineWidth.setDirty();
    }

    Value<StencilFunc> stencilFunc;
    Value<StencilMask> stencilMask;
    Value<StencilTest> stencilTest;
    Value<StencilOp> stencilOp;
    Value<DepthRange> depthRange;
    Value<DepthMask> depthMask;
    Value<DepthTest> depthTest;
    Value<DepthFunc> depthFunc;
    Value<Blend> blend;
    Value<BlendFunc> blendFunc;
    Value<ColorMask> colorMask;
    Value<ClearDepth> clearDepth;
    Value<ClearColor> clearColor;
    Value<ClearStencil> clearStencil;
    Value<Program> program;
    Value<LineWidth> lineWidth;
};

} // namespace gl
} // namespace mbgl

#endif // MBGL_RENDERER_GL_CONFIG
