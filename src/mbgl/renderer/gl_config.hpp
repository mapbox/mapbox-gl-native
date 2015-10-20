#ifndef MBGL_RENDERER_GL_CONFIG
#define MBGL_RENDERER_GL_CONFIG

#include <cstdint>
#include <tuple>
#include <array>

#include <mbgl/platform/gl.hpp>

namespace mbgl {
namespace gl {

template <typename T>
class Value {
public:
    inline void operator=(const typename T::Type& value) {
        if (current != value) {
            current = value;
            T::Set(current);
        }
    }

    inline void reset() {
        current = T::Default;
        T::Set(current);
    }

    inline void save() {
        current = T::Get();
    }

    inline void restore() {
        T::Set(current);
    }

private:
    typename T::Type current = T::Default;
};

struct ClearDepth {
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

struct ClearColor {
    struct Type { GLfloat r, g, b, a; };
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

struct ClearStencil {
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

struct StencilMask {
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

struct DepthMask {
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

struct ColorMask {
    struct Type { bool r, g, b, a; };
    static const Type Default;
    inline static void Set(const Type& value) {
        MBGL_CHECK_ERROR(glColorMask(value.r, value.g, value.b, value.a));
    }
    inline static Type Get() {
        GLfloat floats[4];
        MBGL_CHECK_ERROR(glGetFloatv(GL_COLOR_WRITEMASK, floats));
        return { floats[0], floats[1], floats[2], floats[3] };
    }
};

inline bool operator!=(const ColorMask::Type& a, const ColorMask::Type& b) {
    return a.r != b.r || a.g != b.g || a.b != b.b || a.a != b.a;
}

struct StencilFunc {
    struct Type { GLenum func; GLint ref; GLuint mask; };
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

struct StencilTest {
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

struct StencilOp {
    struct Type { GLenum sfail, dpfail, dppass; };
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

struct DepthRange {
    struct Type { GLfloat near, far; };
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

struct DepthTest {
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

struct DepthFunc {
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

struct Blend {
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

struct BlendFunc {
    struct Type { GLenum sfactor, dfactor; };
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
    }

    void restore() {
        stencilFunc.restore();
        stencilMask.restore();
        stencilTest.restore();
        stencilOp.restore();
        depthRange.restore();
        depthMask.restore();
        depthTest.restore();
        depthFunc.restore();
        blend.restore();
        blendFunc.restore();
        colorMask.restore();
        clearDepth.restore();
        clearColor.restore();
        clearStencil.restore();
    }

    void save() {
        stencilFunc.save();
        stencilMask.save();
        stencilTest.save();
        stencilOp.save();
        depthRange.save();
        depthMask.save();
        depthTest.save();
        depthFunc.save();
        blend.save();
        blendFunc.save();
        colorMask.save();
        clearDepth.save();
        clearColor.save();
        clearStencil.save();
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
};

} // namespace gl
} // namespace mbgl

#endif // MBGL_RENDERER_GL_CONFIG
