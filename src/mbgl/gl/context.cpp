#include <mbgl/map/view.hpp>
#include <mbgl/gl/context.hpp>
#include <mbgl/gl/gl.hpp>
#include <mbgl/gl/vertex_array.hpp>
#include <mbgl/util/traits.hpp>
#include <mbgl/util/std.hpp>
#include <mbgl/util/logging.hpp>

#include <cstring>

namespace mbgl {
namespace gl {

static_assert(underlying_type(ShaderType::Vertex) == GL_VERTEX_SHADER, "OpenGL type mismatch");
static_assert(underlying_type(ShaderType::Fragment) == GL_FRAGMENT_SHADER, "OpenGL type mismatch");

static_assert(underlying_type(PrimitiveType::Points) == GL_POINTS, "OpenGL type mismatch");
static_assert(underlying_type(PrimitiveType::Lines) == GL_LINES, "OpenGL type mismatch");
static_assert(underlying_type(PrimitiveType::LineLoop) == GL_LINE_LOOP, "OpenGL type mismatch");
static_assert(underlying_type(PrimitiveType::LineStrip) == GL_LINE_STRIP, "OpenGL type mismatch");
static_assert(underlying_type(PrimitiveType::Triangles) == GL_TRIANGLES, "OpenGL type mismatch");
static_assert(underlying_type(PrimitiveType::TriangleStrip) == GL_TRIANGLE_STRIP, "OpenGL type mismatch");
static_assert(underlying_type(PrimitiveType::TriangleFan) == GL_TRIANGLE_FAN, "OpenGL type mismatch");

static_assert(std::is_same<ProgramID, GLuint>::value, "OpenGL type mismatch");
static_assert(std::is_same<ShaderID, GLuint>::value, "OpenGL type mismatch");
static_assert(std::is_same<BufferID, GLuint>::value, "OpenGL type mismatch");
static_assert(std::is_same<TextureID, GLuint>::value, "OpenGL type mismatch");
static_assert(std::is_same<VertexArrayID, GLuint>::value, "OpenGL type mismatch");
static_assert(std::is_same<FramebufferID, GLuint>::value, "OpenGL type mismatch");
static_assert(std::is_same<RenderbufferID, GLuint>::value, "OpenGL type mismatch");

static_assert(std::is_same<std::underlying_type_t<TextureFormat>, GLenum>::value, "OpenGL type mismatch");
static_assert(underlying_type(TextureFormat::RGBA) == GL_RGBA, "OpenGL type mismatch");
static_assert(underlying_type(TextureFormat::Alpha) == GL_ALPHA, "OpenGL type mismatch");

Context::~Context() {
    reset();
}

UniqueShader Context::createShader(ShaderType type, const std::string& source) {
    UniqueShader result { MBGL_CHECK_ERROR(glCreateShader(static_cast<GLenum>(type))), { this } };

    const GLchar* sources = source.data();
    const GLsizei lengths = static_cast<GLsizei>(source.length());
    MBGL_CHECK_ERROR(glShaderSource(result, 1, &sources, &lengths));
    MBGL_CHECK_ERROR(glCompileShader(result));

    GLint status = 0;
    MBGL_CHECK_ERROR(glGetShaderiv(result, GL_COMPILE_STATUS, &status));
    if (status != 0) {
        return result;
    }

    GLint logLength;
    MBGL_CHECK_ERROR(glGetShaderiv(result, GL_INFO_LOG_LENGTH, &logLength));
    if (logLength > 0) {
        const auto log = std::make_unique<GLchar[]>(logLength);
        MBGL_CHECK_ERROR(glGetShaderInfoLog(result, logLength, &logLength, log.get()));
        Log::Error(Event::Shader, "Shader failed to compile: %s", log.get());
    }

    throw std::runtime_error("shader failed to compile");
}

UniqueProgram Context::createProgram(ShaderID vertexShader, ShaderID fragmentShader) {
    UniqueProgram result { MBGL_CHECK_ERROR(glCreateProgram()), { this } };

    MBGL_CHECK_ERROR(glAttachShader(result, vertexShader));
    MBGL_CHECK_ERROR(glAttachShader(result, fragmentShader));

    return result;
}

void Context::linkProgram(ProgramID program_) {
    MBGL_CHECK_ERROR(glLinkProgram(program_));

    GLint status;
    MBGL_CHECK_ERROR(glGetProgramiv(program_, GL_LINK_STATUS, &status));
    if (status == GL_TRUE) {
        return;
    }

    GLint logLength;
    MBGL_CHECK_ERROR(glGetProgramiv(program_, GL_INFO_LOG_LENGTH, &logLength));
    const auto log = std::make_unique<GLchar[]>(logLength);
    if (logLength > 0) {
        MBGL_CHECK_ERROR(glGetProgramInfoLog(program_, logLength, &logLength, log.get()));
        Log::Error(Event::Shader, "Program failed to link: %s", log.get());
    }

    throw std::runtime_error("program failed to link");
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
    vertexArrayObject = 0;
    elementBuffer = result;
    MBGL_CHECK_ERROR(glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, GL_STATIC_DRAW));
    return result;
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

UniqueFramebuffer Context::createFramebuffer() {
    FramebufferID id = 0;
    MBGL_CHECK_ERROR(glGenFramebuffers(1, &id));
    return UniqueFramebuffer{ std::move(id), { this } };
}

UniqueRenderbuffer Context::createRenderbuffer(const RenderbufferType type, const Size size) {
    RenderbufferID id = 0;
    MBGL_CHECK_ERROR(glGenRenderbuffers(1, &id));
    UniqueRenderbuffer renderbuffer{ std::move(id), { this } };

    bindRenderbuffer = renderbuffer;
    MBGL_CHECK_ERROR(
        glRenderbufferStorage(GL_RENDERBUFFER, static_cast<GLenum>(type), size.width, size.height));
    return renderbuffer;
}

std::unique_ptr<uint8_t[]> Context::readFramebuffer(const Size size, const TextureFormat format, const bool flip) {
    const size_t stride = size.width * (format == TextureFormat::RGBA ? 4 : 1);
    auto data = std::make_unique<uint8_t[]>(stride * size.height);

#if not MBGL_USE_GLES2
    // When reading data from the framebuffer, make sure that we are storing the values
    // tightly packed into the buffer to avoid buffer overruns.
    pixelStorePack = { 1 };
#endif // MBGL_USE_GLES2

    MBGL_CHECK_ERROR(glReadPixels(0, 0, size.width, size.height, static_cast<GLenum>(format),
                                  GL_UNSIGNED_BYTE, data.get()));

    if (flip) {
        auto tmp = std::make_unique<uint8_t[]>(stride);
        uint8_t* rgba = data.get();
        for (int i = 0, j = size.height - 1; i < j; i++, j--) {
            std::memcpy(tmp.get(), rgba + i * stride, stride);
            std::memcpy(rgba + i * stride, rgba + j * stride, stride);
            std::memcpy(rgba + j * stride, tmp.get(), stride);
        }
    }

    return data;
}

#if not MBGL_USE_GLES2
void Context::drawPixels(const Size size, const void* data, TextureFormat format) {
    pixelStoreUnpack = { 1 };
    if (format != TextureFormat::RGBA) {
        format = static_cast<TextureFormat>(GL_LUMINANCE);
    }
    MBGL_CHECK_ERROR(glDrawPixels(size.width, size.height, static_cast<GLenum>(GL_LUMINANCE),
                                  GL_UNSIGNED_BYTE, data));
}
#endif // MBGL_USE_GLES2

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
Context::createTexture(const Size size, const void* data, TextureFormat format, TextureUnit unit) {
    auto obj = createTexture();
    updateTexture(obj, size, data, format, unit);
    // We are using clamp to edge here since OpenGL ES doesn't allow GL_REPEAT on NPOT textures.
    // We use those when the pixelRatio isn't a power of two, e.g. on iPhone 6 Plus.
    MBGL_CHECK_ERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    MBGL_CHECK_ERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
    MBGL_CHECK_ERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
    MBGL_CHECK_ERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
    return obj;
}

void Context::updateTexture(
    TextureID id, const Size size, const void* data, TextureFormat format, TextureUnit unit) {
    activeTexture = unit;
    texture[unit] = id;
    MBGL_CHECK_ERROR(glTexImage2D(GL_TEXTURE_2D, 0, static_cast<GLenum>(format), size.width,
                                  size.height, 0, static_cast<GLenum>(format), GL_UNSIGNED_BYTE,
                                  data));
}

void Context::bindTexture(Texture& obj,
                          TextureUnit unit,
                          TextureFilter filter,
                          TextureMipMap mipmap,
                          TextureWrap wrapX,
                          TextureWrap wrapY) {
    if (filter != obj.filter || mipmap != obj.mipmap || wrapX != obj.wrapX || wrapY != obj.wrapY) {
        activeTexture = unit;
        texture[unit] = obj.texture;

        if (filter != obj.filter || mipmap != obj.mipmap) {
            MBGL_CHECK_ERROR(glTexParameteri(
                GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                filter == TextureFilter::Linear
                    ? (mipmap == TextureMipMap::Yes ? GL_LINEAR_MIPMAP_NEAREST : GL_LINEAR)
                    : (mipmap == TextureMipMap::Yes ? GL_NEAREST_MIPMAP_NEAREST : GL_NEAREST)));
            MBGL_CHECK_ERROR(
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
                                filter == TextureFilter::Linear ? GL_LINEAR : GL_NEAREST));
            obj.filter = filter;
            obj.mipmap = mipmap;
        }
        if (wrapX != obj.wrapX) {

            MBGL_CHECK_ERROR(
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
                                wrapX == TextureWrap::Clamp ? GL_CLAMP_TO_EDGE : GL_REPEAT));
            obj.wrapX = wrapX;
        }
        if (wrapY != obj.wrapY) {
            MBGL_CHECK_ERROR(
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,
                                wrapY == TextureWrap::Clamp ? GL_CLAMP_TO_EDGE : GL_REPEAT));
            obj.wrapY = wrapY;
        }
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
    blendEquation.setDirty();
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
    pointSize.setDirty();
    pixelZoom.setDirty();
    rasterPos.setDirty();
    pixelStorePack.setDirty();
    pixelStoreUnpack.setDirty();
    pixelTransferDepth.setDirty();
    pixelTransferStencil.setDirty();
#endif // MBGL_USE_GLES2
    for (auto& tex : texture) {
       tex.setDirty();
    }
    vertexBuffer.setDirty();
    elementBuffer.setDirty();
    vertexArrayObject.setDirty();
}

void Context::clear(optional<mbgl::Color> color,
                    optional<float> depth,
                    optional<int32_t> stencil) {
    GLbitfield mask = 0;

    if (color) {
        mask |= GL_COLOR_BUFFER_BIT;
        clearColor = *color;
        colorMask = { true, true, true, true };
    }

    if (depth) {
        mask |= GL_DEPTH_BUFFER_BIT;
        clearDepth = *depth;
        depthMask = true;
    }

    if (stencil) {
        mask |= GL_STENCIL_BUFFER_BIT;
        clearStencil = *stencil;
        stencilMask = 0xFF;
    }

    MBGL_CHECK_ERROR(glClear(mask));
}

#if not MBGL_USE_GLES2
PrimitiveType Context::operator()(const Points& points) {
    pointSize = points.pointSize;
    return PrimitiveType::Points;
}
#else
PrimitiveType Context::operator()(const Points&) {
    return PrimitiveType::Points;
}
#endif // MBGL_USE_GLES2

PrimitiveType Context::operator()(const Lines& lines) {
    lineWidth = lines.lineWidth;
    return PrimitiveType::Lines;
}

PrimitiveType Context::operator()(const LineStrip& lineStrip) {
    lineWidth = lineStrip.lineWidth;
    return PrimitiveType::LineStrip;
}

PrimitiveType Context::operator()(const Triangles&) {
    return PrimitiveType::Triangles;
}

PrimitiveType Context::operator()(const TriangleStrip&) {
    return PrimitiveType::TriangleStrip;
}

void Context::setDepthMode(const DepthMode& depth) {
    if (depth.func == DepthMode::Always && !depth.mask) {
        depthTest = false;
    } else {
        depthTest = true;
        depthFunc = depth.func;
        depthMask = depth.mask;
        depthRange = depth.range;
    }
}

void Context::setStencilMode(const StencilMode& stencil) {
    if (stencil.test.is<StencilMode::Always>() && !stencil.mask) {
        stencilTest = false;
    } else {
        stencilTest = true;
        stencilMask = stencil.mask;
        stencilOp = { stencil.fail, stencil.depthFail, stencil.pass };
        apply_visitor([&] (const auto& test) {
            stencilFunc = { test.func, stencil.ref, test.mask };
        }, stencil.test);
    }
}

void Context::setColorMode(const ColorMode& color) {
    if (color.blendFunction.is<ColorMode::Replace>()) {
        blend = false;
    } else {
        blend = true;
        blendColor = color.blendColor;
        apply_visitor([&] (const auto& blendFunction) {
            blendEquation = ColorMode::BlendEquation(blendFunction.equation);
            blendFunc = { blendFunction.srcFactor, blendFunction.dstFactor };
        }, color.blendFunction);
    }

    colorMask = color.mask;
}

void Context::draw(const Drawable& drawable) {
    if (drawable.segments.empty()) {
        return;
    }

    PrimitiveType primitiveType = apply_visitor([&] (auto m) { return (*this)(m); }, drawable.drawMode);

    setDepthMode(drawable.depthMode);
    setStencilMode(drawable.stencilMode);
    setColorMode(drawable.colorMode);

    program = drawable.program;

    drawable.bindUniforms();

    for (const auto& segment : drawable.segments) {
        auto needAttributeBindings = [&] () {
            if (!gl::GenVertexArrays || !gl::BindVertexArray) {
                return true;
            }

            if (segment.vao) {
                vertexArrayObject = *segment.vao;
                return false;
            }

            VertexArrayID id = 0;
            MBGL_CHECK_ERROR(gl::GenVertexArrays(1, &id));
            vertexArrayObject = id;
            segment.vao = UniqueVertexArray(std::move(id), { this });

            // If we are initializing a new VAO, we need to force the buffers
            // to be rebound. VAOs don't inherit the existing buffer bindings.
            vertexBuffer.setDirty();
            elementBuffer.setDirty();

            return true;
        };

        if (needAttributeBindings()) {
            vertexBuffer = drawable.vertexBuffer;
            elementBuffer = drawable.indexBuffer;
            drawable.bindAttributes(segment.vertexOffset);
        }

        MBGL_CHECK_ERROR(glDrawElements(
            static_cast<GLenum>(primitiveType),
            static_cast<GLsizei>(segment.indexLength),
            GL_UNSIGNED_SHORT,
            reinterpret_cast<GLvoid*>(sizeof(uint16_t) * segment.indexOffset)));
    }
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
