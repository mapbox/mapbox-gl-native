#include <mbgl/map/view.hpp>
#include <mbgl/gl/context.hpp>
#include <mbgl/gl/gl.hpp>
#include <mbgl/gl/vertex_array.hpp>
#include <mbgl/util/traits.hpp>

namespace mbgl {
namespace gl {

static_assert(std::is_same<ProgramID, GLuint>::value, "OpenGL type mismatch");
static_assert(std::is_same<ShaderID, GLuint>::value, "OpenGL type mismatch");
static_assert(std::is_same<BufferID, GLuint>::value, "OpenGL type mismatch");
static_assert(std::is_same<TextureID, GLuint>::value, "OpenGL type mismatch");
static_assert(std::is_same<VertexArrayID, GLuint>::value, "OpenGL type mismatch");
static_assert(std::is_same<FramebufferID, GLuint>::value, "OpenGL type mismatch");
static_assert(std::is_same<RenderbufferID, GLuint>::value, "OpenGL type mismatch");

static_assert(std::is_same<StencilValue, GLint>::value, "OpenGL type mismatch");
static_assert(std::is_same<StencilMaskValue, GLuint>::value, "OpenGL type mismatch");

static_assert(underlying_type(StencilTestFunction::Never) == GL_NEVER, "OpenGL enum mismatch");
static_assert(underlying_type(StencilTestFunction::Less) == GL_LESS, "OpenGL enum mismatch");
static_assert(underlying_type(StencilTestFunction::Equal) == GL_EQUAL, "OpenGL enum mismatch");
static_assert(underlying_type(StencilTestFunction::LessEqual) == GL_LEQUAL, "OpenGL enum mismatch");
static_assert(underlying_type(StencilTestFunction::Greater) == GL_GREATER, "OpenGL enum mismatch");
static_assert(underlying_type(StencilTestFunction::NotEqual) == GL_NOTEQUAL, "OpenGL enum mismatch");
static_assert(underlying_type(StencilTestFunction::GreaterEqual) == GL_GEQUAL, "OpenGL enum mismatch");
static_assert(underlying_type(StencilTestFunction::Always) == GL_ALWAYS, "OpenGL enum mismatch");

static_assert(underlying_type(StencilTestOperation::Keep) == GL_KEEP, "OpenGL enum mismatch");
static_assert(underlying_type(StencilTestOperation::Zero) == GL_ZERO, "OpenGL enum mismatch");
static_assert(underlying_type(StencilTestOperation::Replace) == GL_REPLACE, "OpenGL enum mismatch");
static_assert(underlying_type(StencilTestOperation::Increment) == GL_INCR, "OpenGL enum mismatch");
static_assert(underlying_type(StencilTestOperation::IncrementWrap) == GL_INCR_WRAP, "OpenGL enum mismatch");
static_assert(underlying_type(StencilTestOperation::Decrement) == GL_DECR, "OpenGL enum mismatch");
static_assert(underlying_type(StencilTestOperation::DecrementWrap) == GL_DECR_WRAP, "OpenGL enum mismatch");
static_assert(underlying_type(StencilTestOperation::Invert) == GL_INVERT, "OpenGL enum mismatch");

static_assert(underlying_type(DepthTestFunction::Never) == GL_NEVER, "OpenGL enum mismatch");
static_assert(underlying_type(DepthTestFunction::Less) == GL_LESS, "OpenGL enum mismatch");
static_assert(underlying_type(DepthTestFunction::Equal) == GL_EQUAL, "OpenGL enum mismatch");
static_assert(underlying_type(DepthTestFunction::LessEqual) == GL_LEQUAL, "OpenGL enum mismatch");
static_assert(underlying_type(DepthTestFunction::Greater) == GL_GREATER, "OpenGL enum mismatch");
static_assert(underlying_type(DepthTestFunction::NotEqual) == GL_NOTEQUAL, "OpenGL enum mismatch");
static_assert(underlying_type(DepthTestFunction::GreaterEqual) == GL_GEQUAL, "OpenGL enum mismatch");
static_assert(underlying_type(DepthTestFunction::Always) == GL_ALWAYS, "OpenGL enum mismatch");

static_assert(underlying_type(BlendSourceFactor::Zero) == GL_ZERO, "OpenGL enum mismatch");
static_assert(underlying_type(BlendSourceFactor::One) == GL_ONE, "OpenGL enum mismatch");
static_assert(underlying_type(BlendSourceFactor::SrcColor) == GL_SRC_COLOR, "OpenGL enum mismatch");
static_assert(underlying_type(BlendSourceFactor::OneMinusSrcColor) == GL_ONE_MINUS_SRC_COLOR, "OpenGL enum mismatch");
static_assert(underlying_type(BlendSourceFactor::DstColor) == GL_DST_COLOR, "OpenGL enum mismatch");
static_assert(underlying_type(BlendSourceFactor::OneMinusDstColor) == GL_ONE_MINUS_DST_COLOR, "OpenGL enum mismatch");
static_assert(underlying_type(BlendSourceFactor::SrcAlpha) == GL_SRC_ALPHA, "OpenGL enum mismatch");
static_assert(underlying_type(BlendSourceFactor::OneMinusSrcAlpha) == GL_ONE_MINUS_SRC_ALPHA, "OpenGL enum mismatch");
static_assert(underlying_type(BlendSourceFactor::DstAlpha) == GL_DST_ALPHA, "OpenGL enum mismatch");
static_assert(underlying_type(BlendSourceFactor::OneMinusDstAlpha) == GL_ONE_MINUS_DST_ALPHA, "OpenGL enum mismatch");
static_assert(underlying_type(BlendSourceFactor::ConstantColor) == GL_CONSTANT_COLOR, "OpenGL enum mismatch");
static_assert(underlying_type(BlendSourceFactor::OneMinusConstantColor) == GL_ONE_MINUS_CONSTANT_COLOR, "OpenGL enum mismatch");
static_assert(underlying_type(BlendSourceFactor::ConstantAlpha) == GL_CONSTANT_ALPHA, "OpenGL enum mismatch");
static_assert(underlying_type(BlendSourceFactor::OneMinusConstantAlpha) == GL_ONE_MINUS_CONSTANT_ALPHA, "OpenGL enum mismatch");
static_assert(underlying_type(BlendSourceFactor::SrcAlphaSaturate) == GL_SRC_ALPHA_SATURATE, "OpenGL enum mismatch");

static_assert(underlying_type(BlendDestinationFactor::Zero) == GL_ZERO, "OpenGL enum mismatch");
static_assert(underlying_type(BlendDestinationFactor::One) == GL_ONE, "OpenGL enum mismatch");
static_assert(underlying_type(BlendDestinationFactor::SrcColor) == GL_SRC_COLOR, "OpenGL enum mismatch");
static_assert(underlying_type(BlendDestinationFactor::OneMinusSrcColor) == GL_ONE_MINUS_SRC_COLOR, "OpenGL enum mismatch");
static_assert(underlying_type(BlendDestinationFactor::DstColor) == GL_DST_COLOR, "OpenGL enum mismatch");
static_assert(underlying_type(BlendDestinationFactor::OneMinusDstColor) == GL_ONE_MINUS_DST_COLOR, "OpenGL enum mismatch");
static_assert(underlying_type(BlendDestinationFactor::SrcAlpha) == GL_SRC_ALPHA, "OpenGL enum mismatch");
static_assert(underlying_type(BlendDestinationFactor::OneMinusSrcAlpha) == GL_ONE_MINUS_SRC_ALPHA, "OpenGL enum mismatch");
static_assert(underlying_type(BlendDestinationFactor::DstAlpha) == GL_DST_ALPHA, "OpenGL enum mismatch");
static_assert(underlying_type(BlendDestinationFactor::OneMinusDstAlpha) == GL_ONE_MINUS_DST_ALPHA, "OpenGL enum mismatch");
static_assert(underlying_type(BlendDestinationFactor::ConstantColor) == GL_CONSTANT_COLOR, "OpenGL enum mismatch");
static_assert(underlying_type(BlendDestinationFactor::OneMinusConstantColor) == GL_ONE_MINUS_CONSTANT_COLOR, "OpenGL enum mismatch");
static_assert(underlying_type(BlendDestinationFactor::ConstantAlpha) == GL_CONSTANT_ALPHA, "OpenGL enum mismatch");
static_assert(underlying_type(BlendDestinationFactor::OneMinusConstantAlpha) == GL_ONE_MINUS_CONSTANT_ALPHA, "OpenGL enum mismatch");

Context::~Context() {
    reset();
}

UniqueProgram Context::createProgram() {
    return UniqueProgram{ MBGL_CHECK_ERROR(glCreateProgram()), { this } };
}

UniqueShader Context::createVertexShader() {
    return UniqueShader{ MBGL_CHECK_ERROR(glCreateShader(GL_VERTEX_SHADER)), { this } };
}

UniqueShader Context::createFragmentShader() {
    return UniqueShader{ MBGL_CHECK_ERROR(glCreateShader(GL_FRAGMENT_SHADER)), { this } };
}

UniqueBuffer Context::createVertexBuffer(const void* data, std::size_t size) {
    BufferID id = 0;
    MBGL_CHECK_ERROR(glGenBuffers(1, &id));
    UniqueBuffer result { std::move(id), { this } };
    vertexBuffer = result;
    MBGL_CHECK_ERROR(glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW));
    return result;
}

UniqueBuffer Context::createIndexBuffer(const void* data, std::size_t size) {
    BufferID id = 0;
    MBGL_CHECK_ERROR(glGenBuffers(1, &id));
    UniqueBuffer result { std::move(id), { this } };
    elementBuffer = result;
    MBGL_CHECK_ERROR(glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, GL_STATIC_DRAW));
    return result;
}

void Context::bindAttribute(const AttributeBinding& binding, std::size_t stride, const int8_t* offset) {
    MBGL_CHECK_ERROR(glEnableVertexAttribArray(binding.location));
    MBGL_CHECK_ERROR(glVertexAttribPointer(binding.location,
                                           binding.count,
                                           static_cast<GLenum>(binding.type),
                                           false,
                                           static_cast<GLsizei>(stride),
                                           offset + binding.offset));
}

UniqueTexture Context::createTexture() {
    if (pooledTextures.empty()) {
        pooledTextures.resize(TextureMax);
        MBGL_CHECK_ERROR(glGenTextures(TextureMax, pooledTextures.data()));
    }

    TextureID id = pooledTextures.back();
    pooledTextures.pop_back();
    return UniqueTexture{ std::move(id), { this } };
}

UniqueVertexArray Context::createVertexArray() {
    VertexArrayID id = 0;
    MBGL_CHECK_ERROR(gl::GenVertexArrays(1, &id));
    return UniqueVertexArray{ std::move(id), { this } };
}

UniqueFramebuffer Context::createFramebuffer() {
    FramebufferID id = 0;
    MBGL_CHECK_ERROR(glGenFramebuffers(1, &id));
    return UniqueFramebuffer{ std::move(id), { this } };
}

UniqueRenderbuffer Context::createRenderbuffer(const RenderbufferType type,
                                               const uint16_t width,
                                               const uint16_t height) {
    RenderbufferID id = 0;
    MBGL_CHECK_ERROR(glGenRenderbuffers(1, &id));
    UniqueRenderbuffer renderbuffer{ std::move(id), { this } };

    bindRenderbuffer = renderbuffer;
    MBGL_CHECK_ERROR(
        glRenderbufferStorage(GL_RENDERBUFFER, static_cast<GLenum>(type), width, height));
    return renderbuffer;
}

namespace {

void checkFramebuffer() {
    GLenum status = MBGL_CHECK_ERROR(glCheckFramebufferStatus(GL_FRAMEBUFFER));
    if (status != GL_FRAMEBUFFER_COMPLETE) {
        switch (status) {
        case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
            throw std::runtime_error("Couldn't create framebuffer: incomplete attachment");
        case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
            throw std::runtime_error("Couldn't create framebuffer: incomplete missing attachment");
#ifdef GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER
        case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
            throw std::runtime_error("Couldn't create framebuffer: incomplete draw buffer");
#endif
#ifdef GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER
        case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
            throw std::runtime_error("Couldn't create framebuffer: incomplete read buffer");
#endif
#ifdef GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS
        case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS:
            throw std::runtime_error("Couldn't create framebuffer: incomplete dimensions");
#endif

        case GL_FRAMEBUFFER_UNSUPPORTED:
            throw std::runtime_error("Couldn't create framebuffer: unsupported");
        default:
            throw std::runtime_error("Couldn't create framebuffer: other");
        }
    }
}

void bindDepthStencilRenderbuffer(
    const Renderbuffer<RenderbufferType::DepthStencil>& depthStencil) {
#ifdef GL_DEPTH_STENCIL_ATTACHMENT
    MBGL_CHECK_ERROR(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
                                               GL_RENDERBUFFER, depthStencil.renderbuffer));
#else
    MBGL_CHECK_ERROR(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER,
                                               depthStencil.renderbuffer));
    MBGL_CHECK_ERROR(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT,
                                               GL_RENDERBUFFER, depthStencil.renderbuffer));
#endif
}

} // namespace

Framebuffer
Context::createFramebuffer(const Renderbuffer<RenderbufferType::RGBA>& color,
                           const Renderbuffer<RenderbufferType::DepthStencil>& depthStencil) {
    if (color.size != depthStencil.size) {
        throw new std::runtime_error("Renderbuffer size mismatch");
    }
    auto fbo = createFramebuffer();
    bindFramebuffer = fbo;
    MBGL_CHECK_ERROR(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                                               GL_RENDERBUFFER, color.renderbuffer));
    bindDepthStencilRenderbuffer(depthStencil);
    checkFramebuffer();
    return { color.size, std::move(fbo) };
}

Framebuffer Context::createFramebuffer(const Renderbuffer<RenderbufferType::RGBA>& color) {
    auto fbo = createFramebuffer();
    bindFramebuffer = fbo;
    MBGL_CHECK_ERROR(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                                               GL_RENDERBUFFER, color.renderbuffer));
    checkFramebuffer();
    return { color.size, std::move(fbo) };
}

Framebuffer
Context::createFramebuffer(const Texture& color,
                           const Renderbuffer<RenderbufferType::DepthStencil>& depthStencil) {
    if (color.size != depthStencil.size) {
        throw new std::runtime_error("Renderbuffer size mismatch");
    }
    auto fbo = createFramebuffer();
    bindFramebuffer = fbo;
    MBGL_CHECK_ERROR(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                                            color.texture, 0));
    bindDepthStencilRenderbuffer(depthStencil);
    checkFramebuffer();
    return { color.size, std::move(fbo) };
}

Framebuffer Context::createFramebuffer(const Texture& color) {
    auto fbo = createFramebuffer();
    bindFramebuffer = fbo;
    MBGL_CHECK_ERROR(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                                            color.texture, 0));
    checkFramebuffer();
    return { color.size, std::move(fbo) };
}

UniqueTexture
Context::createTexture(uint16_t width, uint16_t height, const void* data, TextureUnit unit) {
    auto obj = createTexture();
    activeTexture = unit;
    texture[unit] = obj;
    MBGL_CHECK_ERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    MBGL_CHECK_ERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
    MBGL_CHECK_ERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
    MBGL_CHECK_ERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
    MBGL_CHECK_ERROR(
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data));
    return obj;
}

void Context::bindTexture(Texture& obj,
                          TextureUnit unit,
                          TextureFilter filter,
                          TextureMipMap mipmap) {
    if (filter != obj.filter || mipmap != obj.mipmap) {
        activeTexture = unit;
        texture[unit] = obj.texture;
        MBGL_CHECK_ERROR(glTexParameteri(
            GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
            filter == TextureFilter::Linear
                ? (mipmap == TextureMipMap::Yes ? GL_LINEAR_MIPMAP_NEAREST : GL_LINEAR)
                : (mipmap == TextureMipMap::Yes ? GL_NEAREST_MIPMAP_NEAREST : GL_NEAREST)));
        MBGL_CHECK_ERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
                                         filter == TextureFilter::Linear ? GL_LINEAR : GL_NEAREST));
        obj.filter = filter;
        obj.mipmap = mipmap;
    } else if (texture[unit] != obj.texture) {
        // We are checking first to avoid setting the active texture without a subsequent
        // texture bind.
        activeTexture = unit;
        texture[unit] = obj.texture;
    }
}

void Context::reset() {
    std::copy(pooledTextures.begin(), pooledTextures.end(), std::back_inserter(abandonedTextures));
    pooledTextures.resize(0);
    performCleanup();
}

void Context::setDirtyState() {
    // Note: does not set viewport/bindFramebuffer to dirty since they are handled separately in
    // the view object.
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
#if not MBGL_USE_GLES2
    pixelZoom.setDirty();
    rasterPos.setDirty();
#endif // MBGL_USE_GLES2
    for (auto& tex : texture) {
       tex.setDirty();
    }
    vertexBuffer.setDirty();
    elementBuffer.setDirty();
    vertexArrayObject.setDirty();
}

void Context::performCleanup() {
    for (auto id : abandonedPrograms) {
        if (program == id) {
            program.setDirty();
        }
        MBGL_CHECK_ERROR(glDeleteProgram(id));
    }
    abandonedPrograms.clear();

    for (auto id : abandonedShaders) {
        MBGL_CHECK_ERROR(glDeleteShader(id));
    }
    abandonedShaders.clear();

    if (!abandonedBuffers.empty()) {
        for (const auto id : abandonedBuffers) {
            if (vertexBuffer == id) {
                vertexBuffer.setDirty();
            } else if (elementBuffer == id) {
                elementBuffer.setDirty();
            }
        }
        MBGL_CHECK_ERROR(glDeleteBuffers(int(abandonedBuffers.size()), abandonedBuffers.data()));
        abandonedBuffers.clear();
    }

    if (!abandonedTextures.empty()) {
        for (const auto id : abandonedTextures) {
            if (activeTexture == id) {
                activeTexture.setDirty();
            }
        }
        MBGL_CHECK_ERROR(glDeleteTextures(int(abandonedTextures.size()), abandonedTextures.data()));
        abandonedTextures.clear();
    }

    if (!abandonedVertexArrays.empty()) {
        for (const auto id : abandonedVertexArrays) {
            if (vertexArrayObject == id) {
                vertexArrayObject.setDirty();
            }
        }
        MBGL_CHECK_ERROR(gl::DeleteVertexArrays(int(abandonedVertexArrays.size()),
                                                abandonedVertexArrays.data()));
        abandonedVertexArrays.clear();
    }

    if (!abandonedFramebuffers.empty()) {
        for (const auto id : abandonedFramebuffers) {
            if (bindFramebuffer == id) {
                bindFramebuffer.setDirty();
            }
        }
        MBGL_CHECK_ERROR(
            glDeleteFramebuffers(int(abandonedFramebuffers.size()), abandonedFramebuffers.data()));
        abandonedFramebuffers.clear();
    }

    if (!abandonedRenderbuffers.empty()) {
        MBGL_CHECK_ERROR(glDeleteRenderbuffers(int(abandonedRenderbuffers.size()),
                                               abandonedRenderbuffers.data()));
        abandonedRenderbuffers.clear();
    }
}

} // namespace gl
} // namespace mbgl
