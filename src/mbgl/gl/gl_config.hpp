#pragma once

#include <cstdint>
#include <tuple>
#include <array>

#include <mbgl/gl/gl_values.hpp>

namespace mbgl {
namespace gl {


template <typename T, typename = void>
struct DefaultValue {
    static typename T::Type Get() {
        return T::Get();
    }
};
template <typename T>
struct DefaultValue<T, decltype((void)T::Default, void())> {
    static typename T::Type Get() {
        return T::Default;
    }
};

template <typename T>
class Value {
public:
    void operator=(const typename T::Type& value) {
        if (*this != value) {
            dirty = false;
            current = value;
            T::Set(current);
        }
    }

    bool operator!=(const typename T::Type& value) const {
        return dirty || current != value;
    }

    void reset() {
        *this = defaultValue;
    }

    void setDirty() {
        dirty = true;
    }

    typename T::Type getCurrent() const {
        return current;
    }

    bool getDirty() const {
        return dirty;
    }

    void setDefaultValue(const typename T::Type& value) {
        defaultValue = value;
    }

private:
    typename T::Type defaultValue = DefaultValue<T>::Get();
    typename T::Type current = defaultValue;
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
    Value<BlendColor> blendColor;
    Value<ColorMask> colorMask;
    Value<ClearDepth> clearDepth;
    Value<ClearColor> clearColor;
    Value<ClearStencil> clearStencil;
    Value<Program> program;
    Value<LineWidth> lineWidth;
    Value<ActiveTexture> activeTexture;
    Value<BindFramebuffer> bindFramebuffer;
    Value<Viewport> viewport;
#ifndef GL_ES_VERSION_2_0
    Value<PixelZoom> pixelZoom;
    Value<RasterPos> rasterPos;
#endif // GL_ES_VERSION_2_0
    std::array<Value<BindTexture>, 2> texture;
};

} // namespace gl
} // namespace mbgl
