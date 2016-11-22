#include <mbgl/gl/value.hpp>
#include <mbgl/gl/gl.hpp>
#include <mbgl/gl/vertex_array.hpp>

namespace mbgl {
namespace gl {
namespace value {

const constexpr ClearDepth::Type ClearDepth::Default;

void ClearDepth::Set(const Type& value) {
#if MBGL_USE_GLES2
    MBGL_CHECK_ERROR(glClearDepthf(value));
#else
    MBGL_CHECK_ERROR(glClearDepth(value));
#endif
}

ClearDepth::Type ClearDepth::Get() {
    GLfloat clearDepth;
    MBGL_CHECK_ERROR(glGetFloatv(GL_DEPTH_CLEAR_VALUE, &clearDepth));
    return clearDepth;
}

const constexpr ClearColor::Type ClearColor::Default;

void ClearColor::Set(const Type& value) {
    MBGL_CHECK_ERROR(glClearColor(value.r, value.g, value.b, value.a));
}

ClearColor::Type ClearColor::Get() {
    GLfloat clearColor[4];
    MBGL_CHECK_ERROR(glGetFloatv(GL_COLOR_CLEAR_VALUE, clearColor));
    return { clearColor[0], clearColor[1], clearColor[2], clearColor[3] };
}

const constexpr ClearStencil::Type ClearStencil::Default;

void ClearStencil::Set(const Type& value) {
    MBGL_CHECK_ERROR(glClearStencil(value));
}

ClearStencil::Type ClearStencil::Get() {
    GLint clearStencil;
    MBGL_CHECK_ERROR(glGetIntegerv(GL_STENCIL_CLEAR_VALUE, &clearStencil));
    return clearStencil;
}

const constexpr StencilMask::Type StencilMask::Default;

void StencilMask::Set(const Type& value) {
    MBGL_CHECK_ERROR(glStencilMask(value));
}

StencilMask::Type StencilMask::Get() {
    GLint stencilMask;
    MBGL_CHECK_ERROR(glGetIntegerv(GL_STENCIL_WRITEMASK, &stencilMask));
    return stencilMask;
}

const constexpr DepthMask::Type DepthMask::Default;

void DepthMask::Set(const Type& value) {
    MBGL_CHECK_ERROR(glDepthMask(value));
}

DepthMask::Type DepthMask::Get() {
    GLboolean depthMask;
    MBGL_CHECK_ERROR(glGetBooleanv(GL_DEPTH_WRITEMASK, &depthMask));
    return depthMask;
}

const constexpr ColorMask::Type ColorMask::Default;

void ColorMask::Set(const Type& value) {
    MBGL_CHECK_ERROR(glColorMask(value.r, value.g, value.b, value.a));
}

ColorMask::Type ColorMask::Get() {
    GLboolean bools[4];
    MBGL_CHECK_ERROR(glGetBooleanv(GL_COLOR_WRITEMASK, bools));
    return { static_cast<bool>(bools[0]), static_cast<bool>(bools[1]), static_cast<bool>(bools[2]),
             static_cast<bool>(bools[3]) };
}

const constexpr StencilFunc::Type StencilFunc::Default;

void StencilFunc::Set(const Type& value) {
    MBGL_CHECK_ERROR(glStencilFunc(static_cast<GLenum>(value.func), value.ref, value.mask));
}

StencilFunc::Type StencilFunc::Get() {
    GLint func, ref, mask;
    MBGL_CHECK_ERROR(glGetIntegerv(GL_STENCIL_FUNC, &func));
    MBGL_CHECK_ERROR(glGetIntegerv(GL_STENCIL_REF, &ref));
    MBGL_CHECK_ERROR(glGetIntegerv(GL_STENCIL_VALUE_MASK, &mask));
    return { static_cast<StencilTestFunction>(func), ref, static_cast<StencilMaskValue>(mask) };
}

const constexpr StencilTest::Type StencilTest::Default;

void StencilTest::Set(const Type& value) {
    MBGL_CHECK_ERROR(value ? glEnable(GL_STENCIL_TEST) : glDisable(GL_STENCIL_TEST));
}

StencilTest::Type StencilTest::Get() {
    Type stencilTest;
    MBGL_CHECK_ERROR(stencilTest = glIsEnabled(GL_STENCIL_TEST));
    return stencilTest;
}

const constexpr StencilOp::Type StencilOp::Default;

void StencilOp::Set(const Type& value) {
    MBGL_CHECK_ERROR(glStencilOp(static_cast<GLenum>(value.sfail),
                                 static_cast<GLenum>(value.dpfail),
                                 static_cast<GLenum>(value.dppass)));
}

StencilOp::Type StencilOp::Get() {
    GLint sfail, dpfail, dppass;
    MBGL_CHECK_ERROR(glGetIntegerv(GL_STENCIL_FAIL, &sfail));
    MBGL_CHECK_ERROR(glGetIntegerv(GL_STENCIL_PASS_DEPTH_FAIL, &dpfail));
    MBGL_CHECK_ERROR(glGetIntegerv(GL_STENCIL_PASS_DEPTH_PASS, &dppass));
    return { static_cast<StencilTestOperation>(sfail), static_cast<StencilTestOperation>(dpfail),
             static_cast<StencilTestOperation>(dppass) };
}

const constexpr DepthRange::Type DepthRange::Default;

void DepthRange::Set(const Type& value) {
#if MBGL_USE_GLES2
    MBGL_CHECK_ERROR(glDepthRangef(value.near, value.far));
#else
    MBGL_CHECK_ERROR(glDepthRange(value.near, value.far));
#endif
}

DepthRange::Type DepthRange::Get() {
    GLfloat floats[2];
    MBGL_CHECK_ERROR(glGetFloatv(GL_DEPTH_RANGE, floats));
    return { floats[0], floats[1] };
}

const constexpr DepthTest::Type DepthTest::Default;

void DepthTest::Set(const Type& value) {
    MBGL_CHECK_ERROR(value ? glEnable(GL_DEPTH_TEST) : glDisable(GL_DEPTH_TEST));
}

DepthTest::Type DepthTest::Get() {
    Type depthTest;
    MBGL_CHECK_ERROR(depthTest = glIsEnabled(GL_DEPTH_TEST));
    return depthTest;
}

const constexpr DepthFunc::Type DepthFunc::Default;

void DepthFunc::Set(const DepthFunc::Type& value) {
    MBGL_CHECK_ERROR(glDepthFunc(static_cast<GLenum>(value)));
}

DepthFunc::Type DepthFunc::Get() {
    GLint depthFunc;
    MBGL_CHECK_ERROR(glGetIntegerv(GL_DEPTH_FUNC, &depthFunc));
    return static_cast<Type>(depthFunc);
}

const constexpr Blend::Type Blend::Default;

void Blend::Set(const Type& value) {
    MBGL_CHECK_ERROR(value ? glEnable(GL_BLEND) : glDisable(GL_BLEND));
}

Blend::Type Blend::Get() {
    Type blend;
    MBGL_CHECK_ERROR(blend = glIsEnabled(GL_BLEND));
    return blend;
}

const constexpr BlendFunc::Type BlendFunc::Default;

void BlendFunc::Set(const Type& value) {
    MBGL_CHECK_ERROR(
        glBlendFunc(static_cast<GLenum>(value.sfactor), static_cast<GLenum>(value.dfactor)));
}

BlendFunc::Type BlendFunc::Get() {
    GLint sfactor, dfactor;
    MBGL_CHECK_ERROR(glGetIntegerv(GL_BLEND_SRC_ALPHA, &sfactor));
    MBGL_CHECK_ERROR(glGetIntegerv(GL_BLEND_DST_ALPHA, &dfactor));
    return { static_cast<BlendSourceFactor>(sfactor),
             static_cast<BlendDestinationFactor>(dfactor) };
}

const constexpr BlendColor::Type BlendColor::Default;

void BlendColor::Set(const Type& value) {
    MBGL_CHECK_ERROR(glBlendColor(value.r, value.g, value.b, value.a));
}

BlendColor::Type BlendColor::Get() {
    GLfloat floats[4];
    MBGL_CHECK_ERROR(glGetFloatv(GL_BLEND_COLOR, floats));
    return { floats[0], floats[1], floats[2], floats[3] };
}

const constexpr Program::Type Program::Default;

void Program::Set(const Type& value) {
    MBGL_CHECK_ERROR(glUseProgram(value));
}

Program::Type Program::Get() {
    GLint program;
    MBGL_CHECK_ERROR(glGetIntegerv(GL_CURRENT_PROGRAM, &program));
    return program;
}

const constexpr LineWidth::Type LineWidth::Default;

void LineWidth::Set(const Type& value) {
    MBGL_CHECK_ERROR(glLineWidth(value));
}

LineWidth::Type LineWidth::Get() {
    GLfloat lineWidth;
    MBGL_CHECK_ERROR(glGetFloatv(GL_LINE_WIDTH, &lineWidth));
    return lineWidth;
}

const constexpr ActiveTexture::Type ActiveTexture::Default;

void ActiveTexture::Set(const Type& value) {
    MBGL_CHECK_ERROR(glActiveTexture(GL_TEXTURE0 + value));
}

ActiveTexture::Type ActiveTexture::Get() {
    GLint activeTexture;
    MBGL_CHECK_ERROR(glGetIntegerv(GL_ACTIVE_TEXTURE, &activeTexture));
    return static_cast<Type>(activeTexture - GL_TEXTURE0);
}

void Viewport::Set(const Type& value) {
    MBGL_CHECK_ERROR(glViewport(value.x, value.y, value.width, value.height));
}

Viewport::Type Viewport::Get() {
    GLint viewport[4];
    MBGL_CHECK_ERROR(glGetIntegerv(GL_VIEWPORT, viewport));
    return { static_cast<int32_t>(viewport[0]), static_cast<int32_t>(viewport[1]),
             static_cast<uint16_t>(viewport[2]), static_cast<uint16_t>(viewport[3]) };
}

void BindFramebuffer::Set(const Type& value) {
    MBGL_CHECK_ERROR(glBindFramebuffer(GL_FRAMEBUFFER, value));
}

BindFramebuffer::Type BindFramebuffer::Get() {
    GLint binding;
    MBGL_CHECK_ERROR(glGetIntegerv(GL_FRAMEBUFFER_BINDING, &binding));
    return binding;
}

const constexpr BindTexture::Type BindTexture::Default;

void BindTexture::Set(const Type& value) {
    MBGL_CHECK_ERROR(glBindTexture(GL_TEXTURE_2D, value));
}

BindTexture::Type BindTexture::Get() {
    GLint binding;
    MBGL_CHECK_ERROR(glGetIntegerv(GL_TEXTURE_BINDING_2D, &binding));
    return binding;
}

const constexpr BindVertexBuffer::Type BindVertexBuffer::Default;

void BindVertexBuffer::Set(const Type& value) {
    MBGL_CHECK_ERROR(glBindBuffer(GL_ARRAY_BUFFER, value));
}

BindVertexBuffer::Type BindVertexBuffer::Get() {
    GLint binding;
    MBGL_CHECK_ERROR(glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &binding));
    return binding;
}

const constexpr BindElementBuffer::Type BindElementBuffer::Default;

void BindElementBuffer::Set(const Type& value) {
    MBGL_CHECK_ERROR(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, value));
}

BindElementBuffer::Type BindElementBuffer::Get() {
    GLint binding;
    MBGL_CHECK_ERROR(glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &binding));
    return binding;
}

const constexpr BindVertexArray::Type BindVertexArray::Default;

void BindVertexArray::Set(const Type& value) {
    if (gl::BindVertexArray) {
        MBGL_CHECK_ERROR(gl::BindVertexArray(value));
    }
}

BindVertexArray::Type BindVertexArray::Get() {
    GLint binding = 0;
    if (gl::BindVertexArray) {
#ifdef GL_VERTEX_ARRAY_BINDING
        MBGL_CHECK_ERROR(glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &binding));
#elif GL_VERTEX_ARRAY_BINDING_OES
        MBGL_CHECK_ERROR(glGetIntegerv(GL_VERTEX_ARRAY_BINDING_OES, &binding));
#elif GL_VERTEX_ARRAY_BINDING_ARB
        MBGL_CHECK_ERROR(glGetIntegerv(GL_VERTEX_ARRAY_BINDING_ARB, &binding));
#elif GLVERTEX_ARRAY_BINDING_APPLE
        MBGL_CHECK_ERROR(glGetIntegerv(GLVERTEX_ARRAY_BINDING_APPLE, &binding));
#endif
    }
    return binding;
}

#if not MBGL_USE_GLES2

const constexpr PixelZoom::Type PixelZoom::Default;

void PixelZoom::Set(const Type& value) {
    MBGL_CHECK_ERROR(glPixelZoom(value.xfactor, value.yfactor));
}

PixelZoom::Type PixelZoom::Get() {
    GLfloat xfactor, yfactor;
    MBGL_CHECK_ERROR(glGetFloatv(GL_ZOOM_X, &xfactor));
    MBGL_CHECK_ERROR(glGetFloatv(GL_ZOOM_Y, &yfactor));
    return { xfactor, yfactor };
}

const constexpr RasterPos::Type RasterPos::Default;

void RasterPos::Set(const Type& value) {
    MBGL_CHECK_ERROR(glRasterPos4d(value.x, value.y, value.z, value.w));
}

RasterPos::Type RasterPos::Get() {
    GLdouble pos[4];
    MBGL_CHECK_ERROR(glGetDoublev(GL_CURRENT_RASTER_POSITION, pos));
    return { pos[0], pos[1], pos[2], pos[3] };
}

#endif // MBGL_USE_GLES2


} // namespace value
} // namespace gl
} // namespace mbgl
