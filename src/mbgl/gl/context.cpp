#include <mbgl/gl/context.hpp>
#include <mbgl/gl/gl.hpp>
#include <mbgl/gl/debugging_extension.hpp>
#include <mbgl/gl/vertex_array_extension.hpp>
#include <mbgl/gl/program_binary_extension.hpp>
#include <mbgl/util/traits.hpp>
#include <mbgl/util/std.hpp>
#include <mbgl/util/logging.hpp>

#include <cstring>

namespace mbgl {
namespace gl {

static_assert(underlying_type(ShaderType::Vertex) == GL_VERTEX_SHADER, "OpenGL type mismatch");
static_assert(underlying_type(ShaderType::Fragment) == GL_FRAGMENT_SHADER, "OpenGL type mismatch");

static_assert(underlying_type(DataType::Byte) == GL_BYTE, "OpenGL type mismatch");
static_assert(underlying_type(DataType::UnsignedByte) == GL_UNSIGNED_BYTE, "OpenGL type mismatch");
static_assert(underlying_type(DataType::Short) == GL_SHORT, "OpenGL type mismatch");
static_assert(underlying_type(DataType::UnsignedShort) == GL_UNSIGNED_SHORT, "OpenGL type mismatch");
static_assert(underlying_type(DataType::Integer) == GL_INT, "OpenGL type mismatch");
static_assert(underlying_type(DataType::UnsignedInteger) == GL_UNSIGNED_INT, "OpenGL type mismatch");
static_assert(underlying_type(DataType::Float) == GL_FLOAT, "OpenGL type mismatch");

#if not MBGL_USE_GLES2
static_assert(underlying_type(RenderbufferType::RGBA) == GL_RGBA8, "OpenGL type mismatch");
#else
static_assert(underlying_type(RenderbufferType::RGBA) == GL_RGBA8_OES, "OpenGL type mismatch");
#endif // MBGL_USE_GLES2
#if not MBGL_USE_GLES2
static_assert(underlying_type(RenderbufferType::DepthStencil) == GL_DEPTH24_STENCIL8, "OpenGL type mismatch");
#else
static_assert(underlying_type(RenderbufferType::DepthStencil) == GL_DEPTH24_STENCIL8_OES, "OpenGL type mismatch");
#endif // MBGL_USE_GLES2
#if not MBGL_USE_GLES2
static_assert(underlying_type(RenderbufferType::DepthComponent) == GL_DEPTH_COMPONENT, "OpenGL type mismatch");
#else
static_assert(underlying_type(RenderbufferType::DepthComponent) == GL_DEPTH_COMPONENT16, "OpenGL type mismatch");
#endif // MBGL_USE_GLES2


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

static_assert(std::is_same<std::underlying_type_t<TextureType>, GLenum>::value, "OpenGL type mismatch");
static_assert(underlying_type(TextureType::UnsignedByte) == GL_UNSIGNED_BYTE, "OpenGL type mismatch");

#if MBGL_USE_GLES2 && GL_HALF_FLOAT_OES
static_assert(underlying_type(TextureType::HalfFloat) == GL_HALF_FLOAT_OES, "OpenGL type mismatch");
#endif
#if !MBGL_USE_GLES2 && GL_HALF_FLOAT_ARB
static_assert(underlying_type(TextureType::HalfFloat) == GL_HALF_FLOAT_ARB, "OpenGL type mismatch");
#endif

static_assert(underlying_type(UniformDataType::Float) == GL_FLOAT, "OpenGL type mismatch");
static_assert(underlying_type(UniformDataType::FloatVec2) == GL_FLOAT_VEC2, "OpenGL type mismatch");
static_assert(underlying_type(UniformDataType::FloatVec3) == GL_FLOAT_VEC3, "OpenGL type mismatch");
static_assert(underlying_type(UniformDataType::FloatVec4) == GL_FLOAT_VEC4, "OpenGL type mismatch");
static_assert(underlying_type(UniformDataType::Int) == GL_INT, "OpenGL type mismatch");
static_assert(underlying_type(UniformDataType::IntVec2) == GL_INT_VEC2, "OpenGL type mismatch");
static_assert(underlying_type(UniformDataType::IntVec3) == GL_INT_VEC3, "OpenGL type mismatch");
static_assert(underlying_type(UniformDataType::IntVec4) == GL_INT_VEC4, "OpenGL type mismatch");
static_assert(underlying_type(UniformDataType::Bool) == GL_BOOL, "OpenGL type mismatch");
static_assert(underlying_type(UniformDataType::BoolVec2) == GL_BOOL_VEC2, "OpenGL type mismatch");
static_assert(underlying_type(UniformDataType::BoolVec3) == GL_BOOL_VEC3, "OpenGL type mismatch");
static_assert(underlying_type(UniformDataType::BoolVec4) == GL_BOOL_VEC4, "OpenGL type mismatch");
static_assert(underlying_type(UniformDataType::FloatMat2) == GL_FLOAT_MAT2, "OpenGL type mismatch");
static_assert(underlying_type(UniformDataType::FloatMat3) == GL_FLOAT_MAT3, "OpenGL type mismatch");
static_assert(underlying_type(UniformDataType::FloatMat4) == GL_FLOAT_MAT4, "OpenGL type mismatch");
static_assert(underlying_type(UniformDataType::Sampler2D) == GL_SAMPLER_2D, "OpenGL type mismatch");
static_assert(underlying_type(UniformDataType::SamplerCube) == GL_SAMPLER_CUBE, "OpenGL type mismatch");

static_assert(underlying_type(BufferUsage::StreamDraw) == GL_STREAM_DRAW, "OpenGL type mismatch");
static_assert(underlying_type(BufferUsage::StaticDraw) == GL_STATIC_DRAW, "OpenGL type mismatch");
static_assert(underlying_type(BufferUsage::DynamicDraw) == GL_DYNAMIC_DRAW, "OpenGL type mismatch");

static_assert(std::is_same<BinaryProgramFormat, GLenum>::value, "OpenGL type mismatch");

Context::Context() = default;

Context::~Context() {
    if (cleanupOnDestruction) {
        reset();
    }
}

void Context::initializeExtensions(const std::function<gl::ProcAddress(const char*)>& getProcAddress) {
    if (const auto* extensions =
            reinterpret_cast<const char*>(MBGL_CHECK_ERROR(glGetString(GL_EXTENSIONS)))) {

        auto fn = [&](
            std::initializer_list<std::pair<const char*, const char*>> probes) -> ProcAddress {
            for (auto probe : probes) {
                if (strstr(extensions, probe.first) != nullptr) {
                    if (ProcAddress ptr = getProcAddress(probe.second)) {
                        return ptr;
                    }
                }
            }
            return nullptr;
        };

        debugging = std::make_unique<extension::Debugging>(fn);
        if (!disableVAOExtension) {
            vertexArray = std::make_unique<extension::VertexArray>(fn);
        }
#if MBGL_HAS_BINARY_PROGRAMS
        programBinary = std::make_unique<extension::ProgramBinary>(fn);
#endif

#if MBGL_USE_GLES2
        constexpr const char* halfFloatExtensionName = "OES_texture_half_float";
        constexpr const char* halfFloatColorBufferExtensionName = "EXT_color_buffer_half_float";
#else
        constexpr const char* halfFloatExtensionName = "ARB_half_float_pixel";
        constexpr const char* halfFloatColorBufferExtensionName = "ARB_color_buffer_float";
#endif
        if (strstr(extensions, halfFloatExtensionName) != nullptr &&
            strstr(extensions, halfFloatColorBufferExtensionName) != nullptr) {

            supportsHalfFloatTextures = true;
        }

        if (!supportsVertexArrays()) {
            Log::Warning(Event::OpenGL, "Not using Vertex Array Objects");
        }
    }
}

void Context::enableDebugging() {
    if (!debugging || !debugging->debugMessageControl || !debugging->debugMessageCallback) {
        return;
    }

    // This will enable all messages including performance hints
    // MBGL_CHECK_ERROR(debugging->debugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE));

    // This will only enable high and medium severity messages
    MBGL_CHECK_ERROR(debugging->debugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_HIGH, 0, nullptr, GL_TRUE));
    MBGL_CHECK_ERROR(debugging->debugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_MEDIUM, 0, nullptr, GL_TRUE));
    MBGL_CHECK_ERROR(debugging->debugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_FALSE));

    MBGL_CHECK_ERROR(debugging->debugMessageCallback(extension::Debugging::DebugCallback, nullptr));
}

UniqueShader Context::createShader(ShaderType type, const std::string& source) {
    UniqueShader result { MBGL_CHECK_ERROR(glCreateShader(static_cast<GLenum>(type))), { this } };

    const GLchar* sources = source.data();
    const auto lengths = static_cast<GLsizei>(source.length());
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

#if MBGL_HAS_BINARY_PROGRAMS
UniqueProgram Context::createProgram(BinaryProgramFormat binaryFormat,
                                     const std::string& binaryProgram) {
    assert(supportsProgramBinaries());
    UniqueProgram result{ MBGL_CHECK_ERROR(glCreateProgram()), { this } };
    MBGL_CHECK_ERROR(programBinary->programBinary(result, static_cast<GLenum>(binaryFormat),
                                                  binaryProgram.data(),
                                                  static_cast<GLint>(binaryProgram.size())));
    verifyProgramLinkage(result);
    return result;
}
#else
UniqueProgram Context::createProgram(BinaryProgramFormat, const std::string&) {
    throw std::runtime_error("binary programs are not supported");
}
#endif

void Context::linkProgram(ProgramID program_) {
    MBGL_CHECK_ERROR(glLinkProgram(program_));
    verifyProgramLinkage(program_);
}

void Context::verifyProgramLinkage(ProgramID program_) {
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

UniqueBuffer Context::createVertexBuffer(const void* data, std::size_t size, const BufferUsage usage) {
    BufferID id = 0;
    MBGL_CHECK_ERROR(glGenBuffers(1, &id));
    UniqueBuffer result { std::move(id), { this } };
    vertexBuffer = result;
    MBGL_CHECK_ERROR(glBufferData(GL_ARRAY_BUFFER, size, data, static_cast<GLenum>(usage)));
    return result;
}

void Context::updateVertexBuffer(UniqueBuffer& buffer, const void* data, std::size_t size) {
    vertexBuffer = buffer;
    MBGL_CHECK_ERROR(glBufferSubData(GL_ARRAY_BUFFER, 0, size, data));
}

UniqueBuffer Context::createIndexBuffer(const void* data, std::size_t size, const BufferUsage usage) {
    BufferID id = 0;
    MBGL_CHECK_ERROR(glGenBuffers(1, &id));
    UniqueBuffer result { std::move(id), { this } };
    bindVertexArray = 0;
    globalVertexArrayState.indexBuffer = result;
    MBGL_CHECK_ERROR(glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, static_cast<GLenum>(usage)));
    return result;
}

void Context::updateIndexBuffer(UniqueBuffer& buffer, const void* data, std::size_t size) {
    // Be sure to unbind any existing vertex array object before binding the index buffer
    // so that we don't mess up another VAO
    bindVertexArray = 0;
    globalVertexArrayState.indexBuffer = buffer;
    MBGL_CHECK_ERROR(glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, size, data));
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

bool Context::supportsVertexArrays() const {
    static bool blacklisted = []() {
        // Blacklist Adreno 2xx, 3xx as it crashes on glBuffer(Sub)Data
        const std::string renderer = reinterpret_cast<const char*>(glGetString(GL_RENDERER));
        return renderer.find("Adreno (TM) 2") != std::string::npos
         || renderer.find("Adreno (TM) 3") != std::string::npos;
    }();

    return !blacklisted &&
           vertexArray &&
           vertexArray->genVertexArrays &&
           vertexArray->bindVertexArray &&
           vertexArray->deleteVertexArrays;
}

#if MBGL_HAS_BINARY_PROGRAMS
bool Context::supportsProgramBinaries() const {
    if (!programBinary || !programBinary->programBinary || !programBinary->getProgramBinary) {
        return false;
    }

    // Blacklist Adreno 3xx, 4xx, and 5xx GPUs due to known bugs:
    // https://bugs.chromium.org/p/chromium/issues/detail?id=510637
    // https://chromium.googlesource.com/chromium/src/gpu/+/master/config/gpu_driver_bug_list.json#2316
    // Blacklist Vivante GC4000 due to bugs when linking loaded programs:
    // https://github.com/mapbox/mapbox-gl-native/issues/10704
    const std::string renderer = reinterpret_cast<const char*>(glGetString(GL_RENDERER));
    if (renderer.find("Adreno (TM) 3") != std::string::npos
     || renderer.find("Adreno (TM) 4") != std::string::npos
     || renderer.find("Adreno (TM) 5") != std::string::npos
     || renderer.find("Vivante GC4000") != std::string::npos) {
        return false;
    }

    return true;
}

optional<std::pair<BinaryProgramFormat, std::string>>
Context::getBinaryProgram(ProgramID program_) const {
    if (!supportsProgramBinaries()) {
        return {};
    }
    GLint binaryLength;
    MBGL_CHECK_ERROR(glGetProgramiv(program_, GL_PROGRAM_BINARY_LENGTH, &binaryLength));
    std::string binary;
    binary.resize(binaryLength);
    GLenum binaryFormat;
    MBGL_CHECK_ERROR(programBinary->getProgramBinary(
        program_, binaryLength, &binaryLength, &binaryFormat, const_cast<char*>(binary.data())));
    if (size_t(binaryLength) != binary.size()) {
        return {};
    }
    return { { binaryFormat, std::move(binary) } };
}
#else
optional<std::pair<BinaryProgramFormat, std::string>> Context::getBinaryProgram(ProgramID) const {
    return {};
}
#endif

VertexArray Context::createVertexArray() {
    if (supportsVertexArrays()) {
        VertexArrayID id = 0;
        MBGL_CHECK_ERROR(vertexArray->genVertexArrays(1, &id));
        UniqueVertexArray vao(std::move(id), { this });
        return { UniqueVertexArrayState(new VertexArrayState(std::move(vao), *this), VertexArrayStateDeleter { true })};
    } else {
        // On GL implementations which do not support vertex arrays, attribute bindings are global state.
        // So return a VertexArray which shares our global state tracking and whose deleter is a no-op.
        return { UniqueVertexArrayState(&globalVertexArrayState, VertexArrayStateDeleter { false }) };
    }
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
    bindRenderbuffer = 0;
    return renderbuffer;
}

std::unique_ptr<uint8_t[]> Context::readFramebuffer(const Size size, const TextureFormat format, const bool flip) {
    const size_t stride = size.width * (format == TextureFormat::RGBA ? 4 : 1);
    auto data = std::make_unique<uint8_t[]>(stride * size.height);

    // When reading data from the framebuffer, make sure that we are storing the values
    // tightly packed into the buffer to avoid buffer overruns.
    pixelStorePack = { 1 };

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
    MBGL_CHECK_ERROR(glDrawPixels(size.width, size.height, static_cast<GLenum>(format),
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
        throw std::runtime_error("Renderbuffer size mismatch");
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
        throw std::runtime_error("Renderbuffer size mismatch");
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

Framebuffer
Context::createFramebuffer(const Texture& color,
                           const Renderbuffer<RenderbufferType::DepthComponent>& depthTarget) {
    if (color.size != depthTarget.size) {
        throw std::runtime_error("Renderbuffer size mismatch");
    }
    auto fbo = createFramebuffer();
    bindFramebuffer = fbo;
    MBGL_CHECK_ERROR(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color.texture, 0));
    MBGL_CHECK_ERROR(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthTarget.renderbuffer));
    checkFramebuffer();
    return { depthTarget.size, std::move(fbo) };
}

UniqueTexture
Context::createTexture(const Size size, const void* data, TextureFormat format, TextureUnit unit, TextureType type) {
    auto obj = createTexture();
    pixelStoreUnpack = { 1 };
    updateTexture(obj, size, data, format, unit, type);
    // We are using clamp to edge here since OpenGL ES doesn't allow GL_REPEAT on NPOT textures.
    // We use those when the pixelRatio isn't a power of two, e.g. on iPhone 6 Plus.
    MBGL_CHECK_ERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    MBGL_CHECK_ERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
    MBGL_CHECK_ERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
    MBGL_CHECK_ERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
    return obj;
}

void Context::updateTexture(
    TextureID id, const Size size, const void* data, TextureFormat format, TextureUnit unit, TextureType type) {
    activeTextureUnit = unit;
    texture[unit] = id;
    MBGL_CHECK_ERROR(glTexImage2D(GL_TEXTURE_2D, 0, static_cast<GLenum>(format), size.width,
                                  size.height, 0, static_cast<GLenum>(format), static_cast<GLenum>(type),
                                  data));
}

void Context::bindTexture(Texture& obj,
                          TextureUnit unit,
                          TextureFilter filter,
                          TextureMipMap mipmap,
                          TextureWrap wrapX,
                          TextureWrap wrapY) {
    if (filter != obj.filter || mipmap != obj.mipmap || wrapX != obj.wrapX || wrapY != obj.wrapY) {
        activeTextureUnit = unit;
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
        activeTextureUnit = unit;
        texture[unit] = obj.texture;
    }
}

void Context::reset() {
    std::copy(pooledTextures.begin(), pooledTextures.end(), std::back_inserter(abandonedTextures));
    pooledTextures.resize(0);
    performCleanup();
}

void Context::setDirtyState() {
    // Note: does not set viewport/scissorTest/bindFramebuffer to dirty
    // since they are handled separately in the view object.
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
    activeTextureUnit.setDirty();
    pixelStorePack.setDirty();
    pixelStoreUnpack.setDirty();
#if not MBGL_USE_GLES2
    pointSize.setDirty();
    pixelZoom.setDirty();
    rasterPos.setDirty();
    pixelTransferDepth.setDirty();
    pixelTransferStencil.setDirty();
#endif // MBGL_USE_GLES2
    for (auto& tex : texture) {
       tex.setDirty();
    }
    vertexBuffer.setDirty();
    bindVertexArray.setDirty();
    globalVertexArrayState.setDirty();
}

void Context::clear(optional<mbgl::Color> color,
                    optional<float> depth,
                    optional<int32_t> stencil) {
    GLbitfield mask = 0;

    if (color) {
        mask |= GL_COLOR_BUFFER_BIT;
        clearColor = *color;
        colorMask = value::ColorMask::Default;
    }

    if (depth) {
        mask |= GL_DEPTH_BUFFER_BIT;
        clearDepth = *depth;
        depthMask = value::DepthMask::Default;
    }

    if (stencil) {
        mask |= GL_STENCIL_BUFFER_BIT;
        clearStencil = *stencil;
        stencilMask = value::StencilMask::Default;
    }

    MBGL_CHECK_ERROR(glClear(mask));
}

#if not MBGL_USE_GLES2
void Context::setDrawMode(const Points& points) {
    pointSize = points.pointSize;
}
#else
void Context::setDrawMode(const Points&) {
}
#endif // MBGL_USE_GLES2

void Context::setDrawMode(const Lines& lines) {
    lineWidth = lines.lineWidth;
}

void Context::setDrawMode(const LineStrip& lineStrip) {
    lineWidth = lineStrip.lineWidth;
}

void Context::setDrawMode(const Triangles&) {
}

void Context::setDrawMode(const TriangleStrip&) {
}

void Context::setDepthMode(const DepthMode& depth) {
    if (depth.func == DepthMode::Always && !depth.mask) {
        depthTest = false;

        // Workaround for rendering errors on Adreno 2xx GPUs. Depth-related state should
        // not matter when the depth test is disabled, but on these GPUs it apparently does.
        // https://github.com/mapbox/mapbox-gl-native/issues/9164
        depthFunc = depth.func;
        depthMask = depth.mask;
        depthRange = depth.range;
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

void Context::draw(PrimitiveType primitiveType,
                   std::size_t indexOffset,
                   std::size_t indexLength) {
    MBGL_CHECK_ERROR(glDrawElements(
        static_cast<GLenum>(primitiveType),
        static_cast<GLsizei>(indexLength),
        GL_UNSIGNED_SHORT,
        reinterpret_cast<GLvoid*>(sizeof(uint16_t) * indexOffset)));
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
            } else if (globalVertexArrayState.indexBuffer == id) {
                globalVertexArrayState.indexBuffer.setDirty();
            }
        }
        MBGL_CHECK_ERROR(glDeleteBuffers(int(abandonedBuffers.size()), abandonedBuffers.data()));
        abandonedBuffers.clear();
    }

    if (!abandonedTextures.empty()) {
        for (const auto id : abandonedTextures) {
            for (auto& binding : texture) {
                if (binding == id) {
                    binding.setDirty();
                }
            }
        }
        MBGL_CHECK_ERROR(glDeleteTextures(int(abandonedTextures.size()), abandonedTextures.data()));
        abandonedTextures.clear();
    }

    if (!abandonedVertexArrays.empty()) {
        assert(supportsVertexArrays());
        for (const auto id : abandonedVertexArrays) {
            if (bindVertexArray == id) {
                bindVertexArray.setDirty();
            }
        }
        MBGL_CHECK_ERROR(vertexArray->deleteVertexArrays(int(abandonedVertexArrays.size()),
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
