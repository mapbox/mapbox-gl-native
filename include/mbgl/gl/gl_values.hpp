#pragma once

#include <cstdint>
#include <tuple>
#include <array>
#include <cassert>

#include <mbgl/gl/gl.hpp>
#include <mbgl/util/color.hpp>

namespace mbgl {
namespace gl {

struct ClearDepth {
    using Type = GLfloat;
    static const Type Default;
    static void Set(const Type& value) {
        MBGL_CHECK_ERROR(glClearDepth(value));
    }
    static Type Get() {
        Type clearDepth;
        MBGL_CHECK_ERROR(glGetFloatv(GL_DEPTH_CLEAR_VALUE, &clearDepth));
        return clearDepth;
    }
};

struct ClearColor {
    using Type = Color;
    static const Type Default;
    static void Set(const Type& value) {
        MBGL_CHECK_ERROR(glClearColor(value.r, value.g, value.b, value.a));
    }
    static Type Get() {
        GLfloat floats[4];
        MBGL_CHECK_ERROR(glGetFloatv(GL_COLOR_CLEAR_VALUE, floats));
        return { floats[0], floats[1], floats[2], floats[3] };
    }
};

struct ClearStencil {
    using Type = GLint;
    static const Type Default;
    static void Set(const Type& value) {
        MBGL_CHECK_ERROR(glClearStencil(value));
    }
    static Type Get() {
        Type clearStencil;
        MBGL_CHECK_ERROR(glGetIntegerv(GL_STENCIL_CLEAR_VALUE, &clearStencil));
        return clearStencil;
    }
};

struct StencilMask {
    using Type = GLuint;
    static const Type Default;
    static void Set(const Type& value) {
        MBGL_CHECK_ERROR(glStencilMask(value));
    }
    static Type Get() {
        GLint stencilMask;
        MBGL_CHECK_ERROR(glGetIntegerv(GL_STENCIL_WRITEMASK, &stencilMask));
        return stencilMask;
    }
};

struct DepthMask {
    using Type = GLboolean;
    static const Type Default;
    static void Set(const Type& value) {
        MBGL_CHECK_ERROR(glDepthMask(value));
    }
    static Type Get() {
        Type depthMask;
        MBGL_CHECK_ERROR(glGetBooleanv(GL_DEPTH_WRITEMASK, &depthMask));
        return depthMask;
    }
};

struct ColorMask {
    struct Type { bool r, g, b, a; };
    static const Type Default;
    static void Set(const Type& value) {
        MBGL_CHECK_ERROR(glColorMask(value.r, value.g, value.b, value.a));
    }
    static Type Get() {
        GLboolean bools[4];
        MBGL_CHECK_ERROR(glGetBooleanv(GL_COLOR_WRITEMASK, bools));
        return { static_cast<bool>(bools[0]), static_cast<bool>(bools[1]),
                 static_cast<bool>(bools[2]), static_cast<bool>(bools[3]) };
    }
};

constexpr bool operator!=(const ColorMask::Type& a, const ColorMask::Type& b) {
    return a.r != b.r || a.g != b.g || a.b != b.b || a.a != b.a;
}

struct StencilFunc {
    struct Type { GLenum func; GLint ref; GLuint mask; };
    static const Type Default;
    static void Set(const Type& value) {
        MBGL_CHECK_ERROR(glStencilFunc(value.func, value.ref, value.mask));
    }
    static Type Get() {
        GLint func, ref, mask;
        MBGL_CHECK_ERROR(glGetIntegerv(GL_STENCIL_FUNC, &func));
        MBGL_CHECK_ERROR(glGetIntegerv(GL_STENCIL_REF, &ref));
        MBGL_CHECK_ERROR(glGetIntegerv(GL_STENCIL_VALUE_MASK, &mask));
        return { static_cast<GLenum>(func), ref, static_cast<GLuint>(mask) };
    }
};

constexpr bool operator!=(const StencilFunc::Type& a, const StencilFunc::Type& b) {
    return a.func != b.func || a.ref != b.ref || a.mask != b.mask;
}

struct StencilTest {
    using Type = bool;
    static const Type Default;
    static void Set(const Type& value) {
        MBGL_CHECK_ERROR(value ? glEnable(GL_STENCIL_TEST) : glDisable(GL_STENCIL_TEST));
    }
    static Type Get() {
        Type stencilTest;
        MBGL_CHECK_ERROR(stencilTest = glIsEnabled(GL_STENCIL_TEST));
        return stencilTest;
    }
};

struct StencilOp {
    struct Type { GLenum sfail, dpfail, dppass; };
    static const Type Default;
    static void Set(const Type& value) {
        MBGL_CHECK_ERROR(glStencilOp(value.sfail, value.dpfail, value.dppass));
    }
    static Type Get() {
        GLint sfail, dpfail, dppass;
        MBGL_CHECK_ERROR(glGetIntegerv(GL_STENCIL_FAIL, &sfail));
        MBGL_CHECK_ERROR(glGetIntegerv(GL_STENCIL_PASS_DEPTH_FAIL, &dpfail));
        MBGL_CHECK_ERROR(glGetIntegerv(GL_STENCIL_PASS_DEPTH_PASS, &dppass));
        return { static_cast<GLenum>(sfail), static_cast<GLenum>(dpfail), static_cast<GLuint>(dppass) };
    }
};

constexpr bool operator!=(const StencilOp::Type& a, const StencilOp::Type& b) {
    return a.sfail != b.sfail || a.dpfail != b.dpfail || a.dppass != b.dppass;
}

struct DepthRange {
    struct Type { GLfloat near, far; };
    static const Type Default;
    static void Set(const Type& value) {
        MBGL_CHECK_ERROR(glDepthRange(value.near, value.far));
    }
    static Type Get() {
        GLfloat floats[2];
        MBGL_CHECK_ERROR(glGetFloatv(GL_DEPTH_RANGE, floats));
        return { floats[0], floats[1] };
    }
};

constexpr bool operator!=(const DepthRange::Type& a, const DepthRange::Type& b) {
    return a.near != b.near || a.far != b.far;
}

struct DepthTest {
    using Type = bool;
    static const Type Default;
    static void Set(const Type& value) {
        MBGL_CHECK_ERROR(value ? glEnable(GL_DEPTH_TEST) : glDisable(GL_DEPTH_TEST));
    }
    static Type Get() {
        Type depthTest;
        MBGL_CHECK_ERROR(depthTest = glIsEnabled(GL_DEPTH_TEST));
        return depthTest;
    }
};

struct DepthFunc {
    using Type = GLenum;
    static const Type Default;
    static void Set(const Type& value) {
        MBGL_CHECK_ERROR(glDepthFunc(value));
    }
    static Type Get() {
        GLint depthFunc;
        MBGL_CHECK_ERROR(glGetIntegerv(GL_DEPTH_FUNC, &depthFunc));
        return depthFunc;
    }
};

struct Blend {
    using Type = bool;
    static const Type Default;
    static void Set(const Type& value) {
        MBGL_CHECK_ERROR(value ? glEnable(GL_BLEND) : glDisable(GL_BLEND));
    }
    static Type Get() {
        Type blend;
        MBGL_CHECK_ERROR(blend = glIsEnabled(GL_BLEND));
        return blend;
    }
};

struct BlendFunc {
    struct Type { GLenum sfactor, dfactor; };
    static const Type Default;
    static void Set(const Type& value) {
        MBGL_CHECK_ERROR(glBlendFunc(value.sfactor, value.dfactor));
    }
    static Type Get() {
        GLint sfactor, dfactor;
        MBGL_CHECK_ERROR(glGetIntegerv(GL_BLEND_SRC_ALPHA, &sfactor));
        MBGL_CHECK_ERROR(glGetIntegerv(GL_BLEND_DST_ALPHA, &dfactor));
        return { static_cast<GLenum>(sfactor), static_cast<GLenum>(dfactor) };
    }
};

constexpr bool operator!=(const BlendFunc::Type& a, const BlendFunc::Type& b) {
    return a.sfactor != b.sfactor || a.dfactor != b.dfactor;
}

struct BlendColor {
    using Type = Color;
    static const Type Default;
    inline static void Set(const Type& value) {
        MBGL_CHECK_ERROR(glBlendColor(value.r, value.g, value.b, value.a));
    }
    inline static Type Get() {
        GLfloat floats[4];
        MBGL_CHECK_ERROR(glGetFloatv(GL_BLEND_COLOR, floats));
        return { floats[0], floats[1], floats[2], floats[3] };
    }
};

struct Program {
    using Type = GLuint;
    static const Type Default;
    static void Set(const Type& value) {
        MBGL_CHECK_ERROR(glUseProgram(value));
    }
    static Type Get() {
        GLint program;
        MBGL_CHECK_ERROR(glGetIntegerv(GL_CURRENT_PROGRAM, &program));
        return program;
    }
};

struct LineWidth {
    using Type = GLfloat;
    static const Type Default;
    static void Set(const Type& value) {
        MBGL_CHECK_ERROR(glLineWidth(value));
    }
    static Type Get() {
        Type lineWidth;
        MBGL_CHECK_ERROR(glGetFloatv(GL_LINE_WIDTH, &lineWidth));
        return lineWidth;
    }
};

struct ActiveTexture {
    using Type = uint8_t;
    static const Type Default;
    static void Set(const Type& value) {
        MBGL_CHECK_ERROR(glActiveTexture(GL_TEXTURE0 + value));
    }
    static Type Get() {
        GLint activeTexture;
        MBGL_CHECK_ERROR(glGetIntegerv(GL_ACTIVE_TEXTURE, &activeTexture));
        return activeTexture - GL_TEXTURE0;
    }
};

struct BindFramebuffer {
    using Type = GLint;
    static void Set(const Type& value) {
        MBGL_CHECK_ERROR(glBindFramebuffer(GL_FRAMEBUFFER, value));
    }
    static Type Get() {
        Type activeFBO;
        MBGL_CHECK_ERROR(glGetIntegerv(GL_FRAMEBUFFER_BINDING, &activeFBO));
        return activeFBO;
    }
};

struct Viewport {
    using Type = std::array<GLint, 4>;
    static void Set(const Type& value) {
        MBGL_CHECK_ERROR(glViewport(value[0], value[1], value[2], value[3]));
    }
    static Type Get() {
        Type pos;
        MBGL_CHECK_ERROR(glGetIntegerv(GL_VIEWPORT, pos.data()));
        return pos;
    }
};


#ifndef GL_ES_VERSION_2_0

struct PixelZoom {
    struct Type { GLfloat xfactor; GLfloat yfactor; };
    static const Type Default;
    static void Set(const Type& value) {
        MBGL_CHECK_ERROR(glPixelZoom(value.xfactor, value.yfactor));
    }
    static Type Get() {
        Type value;
        MBGL_CHECK_ERROR(glGetFloatv(GL_ZOOM_X, &value.xfactor));
        MBGL_CHECK_ERROR(glGetFloatv(GL_ZOOM_Y, &value.yfactor));
        return value;
    }
};

constexpr bool operator!=(const PixelZoom::Type& a, const PixelZoom::Type& b) {
    return a.xfactor != b.xfactor || a.yfactor != b.yfactor;
}

struct RasterPos {
    using Type = std::array<GLdouble, 4>;
    static const Type Default;
    static void Set(const Type& value) {
        MBGL_CHECK_ERROR(glRasterPos4d(value[0], value[1], value[2], value[3]));
    }
    static Type Get() {
        Type pos;
        MBGL_CHECK_ERROR(glGetDoublev(GL_CURRENT_RASTER_POSITION, pos.data()));
        return pos;
    }
};

#endif // GL_ES_VERSION_2_0

struct BindTexture {
    using Type = GLuint;
    static const Type Default;
    static void Set(const Type& value) {
        MBGL_CHECK_ERROR(glBindTexture(GL_TEXTURE_2D, value));
    }
    static Type Get() {
        GLint texture;
        MBGL_CHECK_ERROR(glGetIntegerv(GL_TEXTURE_BINDING_2D, &texture));
        return texture;
    }
};

} // namespace gl
} // namespace mbgl
