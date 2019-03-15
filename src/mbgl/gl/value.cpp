#include <mbgl/gl/value.hpp>
#include <mbgl/gl/context.hpp>
#include <mbgl/gl/vertex_buffer_resource.hpp>
#include <mbgl/gl/vertex_array_extension.hpp>
#include <mbgl/gl/enum.hpp>

namespace mbgl {
namespace gl {
namespace value {

using namespace platform;

const constexpr ClearDepth::Type ClearDepth::Default;

void ClearDepth::Set(const Type& value) {
    MBGL_CHECK_ERROR(glClearDepthf(value));
}

ClearDepth::Type ClearDepth::Get() {
    GLfloat clearDepth;
    MBGL_CHECK_ERROR(glGetFloatv(GL_DEPTH_CLEAR_VALUE, &clearDepth));
    return clearDepth;
}

const ClearColor::Type ClearColor::Default { 0, 0, 0, 0 };

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
    MBGL_CHECK_ERROR(glDepthMask(Enum<gfx::DepthMaskType>::to(value)));
}

DepthMask::Type DepthMask::Get() {
    GLboolean depthMask;
    MBGL_CHECK_ERROR(glGetBooleanv(GL_DEPTH_WRITEMASK, &depthMask));
    return Enum<gfx::DepthMaskType>::from(depthMask);
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
    MBGL_CHECK_ERROR(glStencilFunc(Enum<gfx::StencilFunctionType>::to(value.func), value.ref, value.mask));
}

StencilFunc::Type StencilFunc::Get() {
    GLint func, ref, mask;
    MBGL_CHECK_ERROR(glGetIntegerv(GL_STENCIL_FUNC, &func));
    MBGL_CHECK_ERROR(glGetIntegerv(GL_STENCIL_REF, &ref));
    MBGL_CHECK_ERROR(glGetIntegerv(GL_STENCIL_VALUE_MASK, &mask));
    return { Enum<gfx::StencilFunctionType>::from(func), ref, static_cast<uint32_t>(mask) };
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
    MBGL_CHECK_ERROR(glStencilOp(Enum<gfx::StencilOpType>::to(value.sfail),
                                 Enum<gfx::StencilOpType>::to(value.dpfail),
                                 Enum<gfx::StencilOpType>::to(value.dppass)));
}

StencilOp::Type StencilOp::Get() {
    GLint sfail, dpfail, dppass;
    MBGL_CHECK_ERROR(glGetIntegerv(GL_STENCIL_FAIL, &sfail));
    MBGL_CHECK_ERROR(glGetIntegerv(GL_STENCIL_PASS_DEPTH_FAIL, &dpfail));
    MBGL_CHECK_ERROR(glGetIntegerv(GL_STENCIL_PASS_DEPTH_PASS, &dppass));
    return { Enum<gfx::StencilOpType>::from(sfail),
             Enum<gfx::StencilOpType>::from(dpfail),
             Enum<gfx::StencilOpType>::from(dppass) };
}

const constexpr DepthRange::Type DepthRange::Default;

void DepthRange::Set(const Type& value) {
    MBGL_CHECK_ERROR(glDepthRangef(value.min, value.max));
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
    MBGL_CHECK_ERROR(glDepthFunc(Enum<gfx::DepthFunctionType>::to(value)));
}

DepthFunc::Type DepthFunc::Get() {
    GLint depthFunc;
    MBGL_CHECK_ERROR(glGetIntegerv(GL_DEPTH_FUNC, &depthFunc));
    return Enum<gfx::DepthFunctionType>::from(depthFunc);
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

const constexpr BlendEquation::Type BlendEquation::Default;

void BlendEquation::Set(const Type& value) {
    MBGL_CHECK_ERROR(glBlendEquation(Enum<gfx::ColorBlendEquationType>::to(value)));
}

BlendEquation::Type BlendEquation::Get() {
    GLint blend;
    MBGL_CHECK_ERROR(glGetIntegerv(GL_BLEND_EQUATION_RGB, &blend));
    return Enum<gfx::ColorBlendEquationType>::from(blend);
}

const constexpr BlendFunc::Type BlendFunc::Default;

void BlendFunc::Set(const Type& value) {
    MBGL_CHECK_ERROR(glBlendFunc(Enum<gfx::ColorBlendFactorType>::to(value.sfactor),
                                 Enum<gfx::ColorBlendFactorType>::to(value.dfactor)));
}

BlendFunc::Type BlendFunc::Get() {
    GLint sfactor, dfactor;
    MBGL_CHECK_ERROR(glGetIntegerv(GL_BLEND_SRC_ALPHA, &sfactor));
    MBGL_CHECK_ERROR(glGetIntegerv(GL_BLEND_DST_ALPHA, &dfactor));
    return { Enum<gfx::ColorBlendFactorType>::from(sfactor),
             Enum<gfx::ColorBlendFactorType>::from(dfactor) };
}

const BlendColor::Type BlendColor::Default { 0, 0, 0, 0 };

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

const constexpr ActiveTextureUnit::Type ActiveTextureUnit::Default;

void ActiveTextureUnit::Set(const Type& value) {
    MBGL_CHECK_ERROR(glActiveTexture(GL_TEXTURE0 + value));
}

ActiveTextureUnit::Type ActiveTextureUnit::Get() {
    GLint activeTexture;
    MBGL_CHECK_ERROR(glGetIntegerv(GL_ACTIVE_TEXTURE, &activeTexture));
    return static_cast<Type>(activeTexture - GL_TEXTURE0);
}

const constexpr Viewport::Type Viewport::Default;

void Viewport::Set(const Type& value) {
    MBGL_CHECK_ERROR(glViewport(value.x, value.y, value.size.width, value.size.height));
}

Viewport::Type Viewport::Get() {
    GLint viewport[4];
    MBGL_CHECK_ERROR(glGetIntegerv(GL_VIEWPORT, viewport));
    return { static_cast<int32_t>(viewport[0]), static_cast<int32_t>(viewport[1]),
             { static_cast<uint32_t>(viewport[2]), static_cast<uint32_t>(viewport[3]) } };
}

const constexpr ScissorTest::Type ScissorTest::Default;

void ScissorTest::Set(const Type& value) {
    MBGL_CHECK_ERROR(value ? glEnable(GL_SCISSOR_TEST) : glDisable(GL_SCISSOR_TEST));
}

ScissorTest::Type ScissorTest::Get() {
    Type scissorTest;
    MBGL_CHECK_ERROR(scissorTest = glIsEnabled(GL_SCISSOR_TEST));
    return scissorTest;
}

const constexpr BindFramebuffer::Type BindFramebuffer::Default;

void BindFramebuffer::Set(const Type& value) {
    MBGL_CHECK_ERROR(glBindFramebuffer(GL_FRAMEBUFFER, value));
}

BindFramebuffer::Type BindFramebuffer::Get() {
    GLint binding;
    MBGL_CHECK_ERROR(glGetIntegerv(GL_FRAMEBUFFER_BINDING, &binding));
    return binding;
}

const constexpr BindRenderbuffer::Type BindRenderbuffer::Default;

void BindRenderbuffer::Set(const Type& value) {
    MBGL_CHECK_ERROR(glBindRenderbuffer(GL_RENDERBUFFER, value));
}

BindRenderbuffer::Type BindRenderbuffer::Get() {
    GLint binding;
    MBGL_CHECK_ERROR(glGetIntegerv(GL_RENDERBUFFER_BINDING, &binding));
    return binding;
}

const constexpr CullFace::Type CullFace::Default;

void CullFace::Set(const Type& value) {
    MBGL_CHECK_ERROR(value ? glEnable(GL_CULL_FACE) : glDisable(GL_CULL_FACE));
}

CullFace::Type CullFace::Get() {
    GLboolean cullFace;
    MBGL_CHECK_ERROR(cullFace = glIsEnabled(GL_CULL_FACE));
    return cullFace;
}

const constexpr CullFaceSide::Type CullFaceSide::Default;

void CullFaceSide::Set(const Type& value) {
    MBGL_CHECK_ERROR(glCullFace(Enum<gfx::CullFaceSideType>::to(value)));
}

CullFaceSide::Type CullFaceSide::Get() {
    GLint cullFaceMode;
    MBGL_CHECK_ERROR(glGetIntegerv(GL_CULL_FACE_MODE, &cullFaceMode));
    return Enum<gfx::CullFaceSideType>::from(cullFaceMode);
}

const constexpr CullFaceWinding::Type CullFaceWinding::Default;

void CullFaceWinding::Set(const Type& value) {
    MBGL_CHECK_ERROR(glFrontFace(Enum<gfx::CullFaceWindingType>::to(value)));
}

CullFaceWinding::Type CullFaceWinding::Get() {
    GLint frontFace;
    MBGL_CHECK_ERROR(glGetIntegerv(GL_FRONT_FACE, &frontFace));
    return Enum<gfx::CullFaceWindingType>::from(frontFace);
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

void BindVertexArray::Set(const Type& value, const Context& context) {
    if (auto vertexArray = context.getVertexArrayExtension()) {
        if (vertexArray->bindVertexArray) {
            MBGL_CHECK_ERROR(vertexArray->bindVertexArray(value));
        }
    }
}

BindVertexArray::Type BindVertexArray::Get(const Context& context) {
    GLint binding = 0;
    if (context.getVertexArrayExtension()) {
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

const VertexAttribute::Type VertexAttribute::Default {};

namespace {

GLenum vertexType(const gfx::AttributeDataType type) {
    switch (type) {
        case gfx::AttributeDataType::Byte:
        case gfx::AttributeDataType::Byte2:
        case gfx::AttributeDataType::Byte3:
        case gfx::AttributeDataType::Byte4:
            return GL_BYTE;
        case gfx::AttributeDataType::UByte:
        case gfx::AttributeDataType::UByte2:
        case gfx::AttributeDataType::UByte3:
        case gfx::AttributeDataType::UByte4:
            return GL_UNSIGNED_BYTE;
        case gfx::AttributeDataType::Short:
        case gfx::AttributeDataType::Short2:
        case gfx::AttributeDataType::Short3:
        case gfx::AttributeDataType::Short4:
            return GL_SHORT;
        case gfx::AttributeDataType::UShort:
        case gfx::AttributeDataType::UShort2:
        case gfx::AttributeDataType::UShort3:
        case gfx::AttributeDataType::UShort4:
            return GL_UNSIGNED_SHORT;
        case gfx::AttributeDataType::Int:
        case gfx::AttributeDataType::Int2:
        case gfx::AttributeDataType::Int3:
        case gfx::AttributeDataType::Int4:
            return GL_INT;
        case gfx::AttributeDataType::UInt:
        case gfx::AttributeDataType::UInt2:
        case gfx::AttributeDataType::UInt3:
        case gfx::AttributeDataType::UInt4:
            return GL_UNSIGNED_INT;
        case gfx::AttributeDataType::Float:
        case gfx::AttributeDataType::Float2:
        case gfx::AttributeDataType::Float3:
        case gfx::AttributeDataType::Float4:
            return GL_FLOAT;
        default:
            return GL_FLOAT;
    }
}

GLint components(const gfx::AttributeDataType type) {
    switch (type) {
        case gfx::AttributeDataType::Byte:
        case gfx::AttributeDataType::UByte:
        case gfx::AttributeDataType::Short:
        case gfx::AttributeDataType::UShort:
        case gfx::AttributeDataType::Int:
        case gfx::AttributeDataType::UInt:
        case gfx::AttributeDataType::Float:
            return 1;
        case gfx::AttributeDataType::Byte2:
        case gfx::AttributeDataType::UByte2:
        case gfx::AttributeDataType::Short2:
        case gfx::AttributeDataType::UShort2:
        case gfx::AttributeDataType::Int2:
        case gfx::AttributeDataType::UInt2:
        case gfx::AttributeDataType::Float2:
            return 2;
        case gfx::AttributeDataType::Byte3:
        case gfx::AttributeDataType::UByte3:
        case gfx::AttributeDataType::Short3:
        case gfx::AttributeDataType::UShort3:
        case gfx::AttributeDataType::Int3:
        case gfx::AttributeDataType::UInt3:
        case gfx::AttributeDataType::Float3:
            return 3;
        case gfx::AttributeDataType::Byte4:
        case gfx::AttributeDataType::UByte4:
        case gfx::AttributeDataType::Short4:
        case gfx::AttributeDataType::UShort4:
        case gfx::AttributeDataType::Int4:
        case gfx::AttributeDataType::UInt4:
        case gfx::AttributeDataType::Float4:
            return 4;
        default:
            return 0;
    }
}

} // namespace

void VertexAttribute::Set(const Type& binding, Context& context, AttributeLocation location) {
    if (binding) {
        context.vertexBuffer = reinterpret_cast<const gl::VertexBufferResource&>(*binding->vertexBufferResource).buffer;
        MBGL_CHECK_ERROR(glEnableVertexAttribArray(location));
        MBGL_CHECK_ERROR(glVertexAttribPointer(
            location,
            components(binding->attribute.dataType),
            vertexType(binding->attribute.dataType),
            static_cast<GLboolean>(false),
            static_cast<GLsizei>(binding->vertexStride),
            reinterpret_cast<GLvoid*>(binding->attribute.offset + (binding->vertexStride * binding->vertexOffset))));
    } else {
        MBGL_CHECK_ERROR(glDisableVertexAttribArray(location));
    }
}

const constexpr PixelStorePack::Type PixelStorePack::Default;

void PixelStorePack::Set(const Type& value) {
    assert(value.alignment == 1 || value.alignment == 2 || value.alignment == 4 ||
           value.alignment == 8);
    MBGL_CHECK_ERROR(glPixelStorei(GL_PACK_ALIGNMENT, value.alignment));
}

PixelStorePack::Type PixelStorePack::Get() {
    Type value;
    MBGL_CHECK_ERROR(glGetIntegerv(GL_PACK_ALIGNMENT, &value.alignment));
    return value;
}

const constexpr PixelStoreUnpack::Type PixelStoreUnpack::Default;

void PixelStoreUnpack::Set(const Type& value) {
    assert(value.alignment == 1 || value.alignment == 2 || value.alignment == 4 ||
           value.alignment == 8);
    MBGL_CHECK_ERROR(glPixelStorei(GL_UNPACK_ALIGNMENT, value.alignment));
}

PixelStoreUnpack::Type PixelStoreUnpack::Get() {
    Type value;
    MBGL_CHECK_ERROR(glGetIntegerv(GL_UNPACK_ALIGNMENT, &value.alignment));
    return value;
}

#if not MBGL_USE_GLES2

const constexpr PointSize::Type PointSize::Default;

void PointSize::Set(const Type& value) {
    MBGL_CHECK_ERROR(glPointSize(value));
}

PointSize::Type PointSize::Get() {
    GLfloat pointSize;
    MBGL_CHECK_ERROR(glGetFloatv(GL_POINT_SIZE, &pointSize));
    return pointSize;
}

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

const constexpr PixelTransferDepth::Type PixelTransferDepth::Default;

void PixelTransferDepth::Set(const Type& value) {
    MBGL_CHECK_ERROR(glPixelTransferf(GL_DEPTH_SCALE, value.scale));
    MBGL_CHECK_ERROR(glPixelTransferf(GL_DEPTH_BIAS, value.bias));
}

PixelTransferDepth::Type PixelTransferDepth::Get() {
    Type value;
    MBGL_CHECK_ERROR(glGetFloatv(GL_DEPTH_SCALE, &value.scale));
    MBGL_CHECK_ERROR(glGetFloatv(GL_DEPTH_BIAS, &value.bias));
    return value;
}

const constexpr PixelTransferStencil::Type PixelTransferStencil::Default;

void PixelTransferStencil::Set(const Type& value) {
    MBGL_CHECK_ERROR(glPixelTransferf(GL_INDEX_SHIFT, value.shift));
    MBGL_CHECK_ERROR(glPixelTransferf(GL_INDEX_OFFSET, value.offset));
}

PixelTransferStencil::Type PixelTransferStencil::Get() {
    Type value;
    MBGL_CHECK_ERROR(glGetIntegerv(GL_INDEX_SHIFT, &value.shift));
    MBGL_CHECK_ERROR(glGetIntegerv(GL_INDEX_OFFSET, &value.offset));
    return value;
}

#endif // MBGL_USE_GLES2

} // namespace value
} // namespace gl
} // namespace mbgl
