#ifndef MBGL_RENDERER_GL_CONFIG
#define MBGL_RENDERER_GL_CONFIG

#include <cstdint>
#include <tuple>
#include <array>

#include <mbgl/platform/gl.hpp>

namespace mbgl {
namespace gl {

template <typename T>
class Value final {
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

struct ClearDepth final {
    using Type = GLfloat;
    static const Type Default;
    inline static void Set(const Type& value) {
        MBGL_CHECK_ERROR(glClearDepth(value));
    }
    inline static Type Get() {
        Type clearDepth;
        MBGL_CHECK_ERROR(glGetFloatv(GL_DEPTH_CLEAR_VALUE, &clearDepth));
        return clearDepth;
    }
};

struct ClearColor final {
    struct Type final { GLfloat r, g, b, a; };
    static const Type Default;
    inline static void Set(const Type& value) {
        MBGL_CHECK_ERROR(glClearColor(value.r, value.g, value.b, value.a));
    }
    inline static Type Get() {
        GLfloat floats[4];
        MBGL_CHECK_ERROR(glGetFloatv(GL_COLOR_CLEAR_VALUE, floats));
        return { floats[0], floats[1], floats[2], floats[3] };
    }
};

inline bool operator!=(const ClearColor::Type& a, const ClearColor::Type& b) {
    return a.r != b.r || a.g != b.g || a.b != b.b || a.a != b.a;
}

struct ClearStencil final {
    using Type = GLint;
    static const Type Default;
    inline static void Set(const Type& value) {
        MBGL_CHECK_ERROR(glClearStencil(value));
    }
    inline static Type Get() {
        Type clearStencil;
        MBGL_CHECK_ERROR(glGetIntegerv(GL_STENCIL_CLEAR_VALUE, &clearStencil));
        return clearStencil;
    }
};

struct StencilMask final {
    using Type = GLuint;
    static const Type Default;
    inline static void Set(const Type& value) {
        MBGL_CHECK_ERROR(glStencilMask(value));
    }
    inline static Type Get() {
        GLint stencilMask;
        MBGL_CHECK_ERROR(glGetIntegerv(GL_STENCIL_WRITEMASK, &stencilMask));
        return stencilMask;
    }
};

struct DepthMask final {
    using Type = GLboolean;
    static const Type Default;
    inline static void Set(const Type& value) {
        MBGL_CHECK_ERROR(glDepthMask(value));
    }
    inline static Type Get() {
        Type depthMask;
        MBGL_CHECK_ERROR(glGetBooleanv(GL_DEPTH_WRITEMASK, &depthMask));
        return depthMask;
    }
};

struct ColorMask final {
    struct Type final { bool r, g, b, a; };
    static const Type Default;
    inline static void Set(const Type& value) {
        MBGL_CHECK_ERROR(glColorMask(value.r, value.g, value.b, value.a));
    }
    inline static Type Get() {
        GLboolean bools[4];
        MBGL_CHECK_ERROR(glGetBooleanv(GL_COLOR_WRITEMASK, bools));
        return { static_cast<bool>(bools[0]), static_cast<bool>(bools[1]),
                 static_cast<bool>(bools[2]), static_cast<bool>(bools[3]) };
    }
};

inline bool operator!=(const ColorMask::Type& a, const ColorMask::Type& b) {
    return a.r != b.r || a.g != b.g || a.b != b.b || a.a != b.a;
}

struct StencilFunc final {
    struct Type final { GLenum func; GLint ref; GLuint mask; };
    static const Type Default;
    inline static void Set(const Type& value) {
        MBGL_CHECK_ERROR(glStencilFunc(value.func, value.ref, value.mask));
    }
    inline static Type Get() {
        GLint func, ref, mask;
        MBGL_CHECK_ERROR(glGetIntegerv(GL_STENCIL_FUNC, &func));
        MBGL_CHECK_ERROR(glGetIntegerv(GL_STENCIL_REF, &ref));
        MBGL_CHECK_ERROR(glGetIntegerv(GL_STENCIL_VALUE_MASK, &mask));
        return { static_cast<GLenum>(func), ref, static_cast<GLuint>(mask) };
    }
};

inline bool operator!=(const StencilFunc::Type& a, const StencilFunc::Type& b) {
    return a.func != b.func || a.ref != b.ref || a.mask != b.mask;
}

struct StencilTest final {
    using Type = bool;
    static const Type Default;
    inline static void Set(const Type& value) {
        MBGL_CHECK_ERROR(value ? glEnable(GL_STENCIL_TEST) : glDisable(GL_STENCIL_TEST));
    }
    inline static Type Get() {
        Type stencilTest;
        MBGL_CHECK_ERROR(stencilTest = glIsEnabled(GL_STENCIL_TEST));
        return stencilTest;
    }
};

struct StencilOp final {
    struct Type final { GLenum sfail, dpfail, dppass; };
    static const Type Default;
    inline static void Set(const Type& value) {
        MBGL_CHECK_ERROR(glStencilOp(value.sfail, value.dpfail, value.dppass));
    }
    inline static Type Get() {
        GLint sfail, dpfail, dppass;
        MBGL_CHECK_ERROR(glGetIntegerv(GL_STENCIL_FAIL, &sfail));
        MBGL_CHECK_ERROR(glGetIntegerv(GL_STENCIL_PASS_DEPTH_FAIL, &dpfail));
        MBGL_CHECK_ERROR(glGetIntegerv(GL_STENCIL_PASS_DEPTH_PASS, &dppass));
        return { static_cast<GLenum>(sfail), static_cast<GLenum>(dpfail), static_cast<GLuint>(dppass) };
    }
};

struct DepthRange final {
    struct Type final { GLfloat near, far; };
    static const Type Default;
    inline static void Set(const Type& value) {
        MBGL_CHECK_ERROR(glDepthRange(value.near, value.far));
    }
    inline static Type Get() {
        GLfloat floats[2];
        MBGL_CHECK_ERROR(glGetFloatv(GL_DEPTH_RANGE, floats));
        return { floats[0], floats[1] };
    }
};

inline bool operator!=(const DepthRange::Type& a, const DepthRange::Type& b) {
    return a.near != b.near || a.far != b.far;
}

struct DepthTest final {
    using Type = bool;
    static const Type Default;
    inline static void Set(const Type& value) {
        MBGL_CHECK_ERROR(value ? glEnable(GL_DEPTH_TEST) : glDisable(GL_DEPTH_TEST));
    }
    inline static Type Get() {
        Type depthTest;
        MBGL_CHECK_ERROR(depthTest = glIsEnabled(GL_DEPTH_TEST));
        return depthTest;
    }
};

struct DepthFunc final {
    using Type = GLenum;
    static const Type Default;
    inline static void Set(const Type& value) {
        MBGL_CHECK_ERROR(glDepthFunc(value));
    }
    inline static Type Get() {
        GLint depthFunc;
        MBGL_CHECK_ERROR(glGetIntegerv(GL_DEPTH_FUNC, &depthFunc));
        return depthFunc;
    }
};

struct Blend final {
    using Type = bool;
    static const Type Default;
    inline static void Set(const Type& value) {
        MBGL_CHECK_ERROR(value ? glEnable(GL_BLEND) : glDisable(GL_BLEND));
    }
    inline static Type Get() {
        Type blend;
        MBGL_CHECK_ERROR(blend = glIsEnabled(GL_BLEND));
        return blend;
    }
};

struct BlendFunc final {
    struct Type final { GLenum sfactor, dfactor; };
    static const Type Default;
    inline static void Set(const Type& value) {
        MBGL_CHECK_ERROR(glBlendFunc(value.sfactor, value.dfactor));
    }
    inline static Type Get() {
        GLint sfactor, dfactor;
        MBGL_CHECK_ERROR(glGetIntegerv(GL_BLEND_SRC_ALPHA, &sfactor));
        MBGL_CHECK_ERROR(glGetIntegerv(GL_BLEND_DST_ALPHA, &dfactor));
        return { static_cast<GLenum>(sfactor), static_cast<GLenum>(dfactor) };
    }
};

struct Program final {
    using Type = GLuint;
    static const Type Default;
    inline static void Set(const Type& value) {
        MBGL_CHECK_ERROR(glUseProgram(value));
    }
    inline static Type Get() {
        GLint program;
        MBGL_CHECK_ERROR(glGetIntegerv(GL_CURRENT_PROGRAM, &program));
        return program;
    }
};

struct LineWidth final {
    using Type = GLfloat;
    static const Type Default;
    inline static void Set(const Type& value) {
        MBGL_CHECK_ERROR(glLineWidth(value));
    }
    inline static Type Get() {
        Type lineWidth;
        MBGL_CHECK_ERROR(glGetFloatv(GL_LINE_WIDTH, &lineWidth));
        return lineWidth;
    }
};

class Config final {
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
