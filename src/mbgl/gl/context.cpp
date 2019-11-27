#include <mbgl/gl/context.hpp>
#include <mbgl/gl/enum.hpp>
#include <mbgl/gl/renderer_backend.hpp>
#include <mbgl/gl/texture_resource.hpp>
#include <mbgl/gl/renderbuffer_resource.hpp>
#include <mbgl/gl/draw_scope_resource.hpp>
#include <mbgl/gl/texture.hpp>
#include <mbgl/gl/offscreen_texture.hpp>
#include <mbgl/gl/command_encoder.hpp>
#include <mbgl/gl/debugging_extension.hpp>
#include <mbgl/gl/vertex_array_extension.hpp>
#include <mbgl/util/traits.hpp>
#include <mbgl/util/std.hpp>
#include <mbgl/util/logging.hpp>

#include <cstring>

namespace mbgl {
namespace gl {

using namespace platform;

static_assert(underlying_type(ShaderType::Vertex) == GL_VERTEX_SHADER, "OpenGL type mismatch");
static_assert(underlying_type(ShaderType::Fragment) == GL_FRAGMENT_SHADER, "OpenGL type mismatch");

static_assert(std::is_same<ProgramID, GLuint>::value, "OpenGL type mismatch");
static_assert(std::is_same<ShaderID, GLuint>::value, "OpenGL type mismatch");
static_assert(std::is_same<BufferID, GLuint>::value, "OpenGL type mismatch");
static_assert(std::is_same<TextureID, GLuint>::value, "OpenGL type mismatch");
static_assert(std::is_same<VertexArrayID, GLuint>::value, "OpenGL type mismatch");
static_assert(std::is_same<FramebufferID, GLuint>::value, "OpenGL type mismatch");
static_assert(std::is_same<RenderbufferID, GLuint>::value, "OpenGL type mismatch");

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

Context::Context(RendererBackend& backend_)
    : gfx::Context([] {
          GLint value;
          MBGL_CHECK_ERROR(glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &value));
          return value;
      }()),
      backend(backend_),
      stats() {}

Context::~Context() {
    if (cleanupOnDestruction) {
        reset();
    }
    assert(stats.isZero());
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

        static const std::string renderer = []() {
            std::string r = reinterpret_cast<const char*>(MBGL_CHECK_ERROR(glGetString(GL_RENDERER)));
            Log::Info(Event::General, "GPU Identifier: %s", r.c_str());
            return r;
        }();

        // Block ANGLE on Direct3D since the debugging extension is causing crashes
        if (!(renderer.find("ANGLE") != std::string::npos
              && renderer.find("Direct3D") != std::string::npos)) {
            debugging = std::make_unique<extension::Debugging>(fn);
        }

        // Block Adreno 2xx, 3xx as it crashes on glBuffer(Sub)Data
        // Block Adreno 4xx as it crashes in a driver when VBOs are destructed (Android 5.1.1)
        // Block ARM Mali-T720 (in some MT8163 chipsets) as it crashes on glBindVertexArray
        // Block ANGLE on Direct3D as the combination of Qt + Windows + ANGLE leads to crashes
        if (renderer.find("Adreno (TM) 2") == std::string::npos &&
            renderer.find("Adreno (TM) 3") == std::string::npos &&
            renderer.find("Adreno (TM) 4") == std::string::npos &&
            (!(renderer.find("ANGLE") != std::string::npos && renderer.find("Direct3D") != std::string::npos)) &&
            renderer.find("Mali-T720") == std::string::npos && renderer.find("Sapphire 650") == std::string::npos &&
            !disableVAOExtension) {
            vertexArray = std::make_unique<extension::VertexArray>(fn);
        }

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

UniqueShader Context::createShader(ShaderType type, const std::initializer_list<const char*>& sources) {
    UniqueShader result { MBGL_CHECK_ERROR(glCreateShader(static_cast<GLenum>(type))), { this } };

    MBGL_CHECK_ERROR(glShaderSource(result, static_cast<GLsizei>(sources.size()), sources.begin(), nullptr));
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

UniqueProgram Context::createProgram(ShaderID vertexShader, ShaderID fragmentShader, const char* location0AttribName) {
    UniqueProgram result { MBGL_CHECK_ERROR(glCreateProgram()), { this } };

    MBGL_CHECK_ERROR(glAttachShader(result, vertexShader));
    MBGL_CHECK_ERROR(glAttachShader(result, fragmentShader));

    // It is important to have attribute at position 0 enabled: conveniently,
    // position attribute is always first and always enabled. The integrity of
    // this assumption is verified in AttributeLocations::queryLocations and
    // AttributeLocations::getFirstAttribName.
    MBGL_CHECK_ERROR(glBindAttribLocation(result, 0, location0AttribName));

    linkProgram(result);

    return result;
}

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

UniqueTexture Context::createUniqueTexture() {
    if (pooledTextures.empty()) {
        pooledTextures.resize(TextureMax);
        MBGL_CHECK_ERROR(glGenTextures(TextureMax, pooledTextures.data()));
        stats.numCreatedTextures += TextureMax;
    }

    TextureID id = pooledTextures.back();
    pooledTextures.pop_back();
    stats.numActiveTextures++;
    // NOLINTNEXTLINE(performance-move-const-arg)
    return UniqueTexture{std::move(id), {this}};
}

bool Context::supportsVertexArrays() const {
    return vertexArray &&
           vertexArray->genVertexArrays &&
           vertexArray->bindVertexArray &&
           vertexArray->deleteVertexArrays;
}

VertexArray Context::createVertexArray() {
    if (supportsVertexArrays()) {
        VertexArrayID id = 0;
        MBGL_CHECK_ERROR(vertexArray->genVertexArrays(1, &id));
        // NOLINTNEXTLINE(performance-move-const-arg)
        UniqueVertexArray vao(std::move(id), { this });
        return { UniqueVertexArrayState(new VertexArrayState(std::move(vao)), VertexArrayStateDeleter { true })};
    } else {
        // On GL implementations which do not support vertex arrays, attribute bindings are global state.
        // So return a VertexArray which shares our global state tracking and whose deleter is a no-op.
        return { UniqueVertexArrayState(&globalVertexArrayState, VertexArrayStateDeleter { false }) };
    }
}

UniqueFramebuffer Context::createFramebuffer() {
    FramebufferID id = 0;
    MBGL_CHECK_ERROR(glGenFramebuffers(1, &id));
    stats.numFrameBuffers++;
    // NOLINTNEXTLINE(performance-move-const-arg)
    return UniqueFramebuffer{ std::move(id), { this } };
}

std::unique_ptr<gfx::TextureResource> Context::createTextureResource(
    const Size size, const gfx::TexturePixelType format, const gfx::TextureChannelDataType type) {
    auto obj = createUniqueTexture();
    int textureByteSize = gl::TextureResource::getStorageSize(size, format, type);
    stats.memTextures += textureByteSize;
    std::unique_ptr<gfx::TextureResource> resource =
        std::make_unique<gl::TextureResource>(std::move(obj), textureByteSize);

    // Always use texture unit 0 for manipulating it.
    activeTextureUnit = 0;
    texture[0] = static_cast<gl::TextureResource&>(*resource).texture;

    // Creates an empty texture with the specified size and format.
    MBGL_CHECK_ERROR(glTexImage2D(GL_TEXTURE_2D, 0, Enum<gfx::TexturePixelType>::to(format),
                                  size.width, size.height, 0,
                                  Enum<gfx::TexturePixelType>::to(format),
                                  Enum<gfx::TextureChannelDataType>::to(type), nullptr));

    // We are using clamp to edge here since OpenGL ES doesn't allow GL_REPEAT on NPOT textures.
    // We use those when the pixelRatio isn't a power of two, e.g. on iPhone 6 Plus.
    MBGL_CHECK_ERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    MBGL_CHECK_ERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
    MBGL_CHECK_ERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
    MBGL_CHECK_ERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));

    return resource;
}

std::unique_ptr<gfx::RenderbufferResource>
Context::createRenderbufferResource(const gfx::RenderbufferPixelType type, const Size size) {
    RenderbufferID id = 0;
    MBGL_CHECK_ERROR(glGenRenderbuffers(1, &id));
    // NOLINTNEXTLINE(performance-move-const-arg)
    UniqueRenderbuffer renderbuffer{ std::move(id), { this } };

    bindRenderbuffer = renderbuffer;
    MBGL_CHECK_ERROR(
        glRenderbufferStorage(GL_RENDERBUFFER, Enum<gfx::RenderbufferPixelType>::to(type), size.width, size.height));
    bindRenderbuffer = 0;
    return std::make_unique<gl::RenderbufferResource>(std::move(renderbuffer));
}


std::unique_ptr<uint8_t[]> Context::readFramebuffer(const Size size, const gfx::TexturePixelType format, const bool flip) {
    const size_t stride = size.width * (format == gfx::TexturePixelType::RGBA ? 4 : 1);
    auto data = std::make_unique<uint8_t[]>(stride * size.height);

    // When reading data from the framebuffer, make sure that we are storing the values
    // tightly packed into the buffer to avoid buffer overruns.
    pixelStorePack = { 1 };

    MBGL_CHECK_ERROR(glReadPixels(0, 0, size.width, size.height,
                                  Enum<gfx::TexturePixelType>::to(format), GL_UNSIGNED_BYTE,
                                  data.get()));

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
void Context::drawPixels(const Size size, const void* data, gfx::TexturePixelType format) {
    pixelStoreUnpack = { 1 };
    // TODO
    if (format != gfx::TexturePixelType::RGBA) {
        format = gfx::TexturePixelType::Luminance;
    }
    MBGL_CHECK_ERROR(glDrawPixels(size.width, size.height, Enum<gfx::TexturePixelType>::to(format),
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
    const gfx::Renderbuffer<gfx::RenderbufferPixelType::DepthStencil>& depthStencil) {
    auto& depthStencilResource = depthStencil.getResource<gl::RenderbufferResource>();
#ifdef GL_DEPTH_STENCIL_ATTACHMENT
    MBGL_CHECK_ERROR(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
                                               GL_RENDERBUFFER, depthStencilResource.renderbuffer));
#else
    MBGL_CHECK_ERROR(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER,
                                               depthStencilResource.renderbuffer));
    MBGL_CHECK_ERROR(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT,
                                               GL_RENDERBUFFER, depthStencilResource.renderbuffer));
#endif
}

} // namespace

Framebuffer
Context::createFramebuffer(const gfx::Renderbuffer<gfx::RenderbufferPixelType::RGBA>& color,
                           const gfx::Renderbuffer<gfx::RenderbufferPixelType::DepthStencil>& depthStencil) {
    if (color.getSize() != depthStencil.getSize()) {
        throw std::runtime_error("Renderbuffer size mismatch");
    }
    auto fbo = createFramebuffer();
    bindFramebuffer = fbo;

    auto& colorResource = color.getResource<gl::RenderbufferResource>();
    MBGL_CHECK_ERROR(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                                               GL_RENDERBUFFER, colorResource.renderbuffer));
    bindDepthStencilRenderbuffer(depthStencil);
    checkFramebuffer();
    return { color.getSize(), std::move(fbo) };
}

Framebuffer Context::createFramebuffer(const gfx::Renderbuffer<gfx::RenderbufferPixelType::RGBA>& color) {
    auto fbo = createFramebuffer();
    bindFramebuffer = fbo;
    auto& colorResource = color.getResource<gl::RenderbufferResource>();
    MBGL_CHECK_ERROR(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                                               GL_RENDERBUFFER, colorResource.renderbuffer));
    checkFramebuffer();
    return { color.getSize(), std::move(fbo) };
}

Framebuffer
Context::createFramebuffer(const gfx::Texture& color,
                           const gfx::Renderbuffer<gfx::RenderbufferPixelType::DepthStencil>& depthStencil) {
    if (color.size != depthStencil.getSize()) {
        throw std::runtime_error("Renderbuffer size mismatch");
    }
    auto fbo = createFramebuffer();
    bindFramebuffer = fbo;
    MBGL_CHECK_ERROR(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                                            color.getResource<gl::TextureResource>().texture, 0));
    bindDepthStencilRenderbuffer(depthStencil);
    checkFramebuffer();
    return { color.size, std::move(fbo) };
}

Framebuffer Context::createFramebuffer(const gfx::Texture& color) {
    auto fbo = createFramebuffer();
    bindFramebuffer = fbo;
    MBGL_CHECK_ERROR(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                                            color.getResource<gl::TextureResource>().texture, 0));
    checkFramebuffer();
    return { color.size, std::move(fbo) };
}

Framebuffer
Context::createFramebuffer(const gfx::Texture& color,
                           const gfx::Renderbuffer<gfx::RenderbufferPixelType::Depth>& depth) {
    if (color.size != depth.getSize()) {
        throw std::runtime_error("Renderbuffer size mismatch");
    }
    auto fbo = createFramebuffer();
    bindFramebuffer = fbo;
    MBGL_CHECK_ERROR(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                                            color.getResource<gl::TextureResource>().texture, 0));

    auto& depthResource = depth.getResource<gl::RenderbufferResource>();
    MBGL_CHECK_ERROR(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER,
                                               depthResource.renderbuffer));
    checkFramebuffer();
    return { depth.getSize(), std::move(fbo) };
}

std::unique_ptr<gfx::OffscreenTexture>
Context::createOffscreenTexture(const Size size, const gfx::TextureChannelDataType type) {
    return std::make_unique<gl::OffscreenTexture>(*this, size, type);
}

std::unique_ptr<gfx::DrawScopeResource> Context::createDrawScopeResource() {
    return std::make_unique<gl::DrawScopeResource>(createVertexArray());
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
    cullFace.setDirty();
    cullFaceSide.setDirty();
    cullFaceWinding.setDirty();
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

    stats.numDrawCalls = 0;
}

void Context::setCullFaceMode(const gfx::CullFaceMode& mode) {
    cullFace = mode.enabled;

    // These shouldn't need to be updated when face culling is disabled, but we
    // might end up having the same isssues with Adreno 2xx GPUs as noted in
    // Context::setDepthMode.
    cullFaceSide = mode.side;
    cullFaceWinding = mode.winding;
}

void Context::setDepthMode(const gfx::DepthMode& depth) {
    if (depth.func == gfx::DepthFunctionType::Always && depth.mask != gfx::DepthMaskType::ReadWrite) {
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

void Context::setStencilMode(const gfx::StencilMode& stencil) {
    if (stencil.test.is<gfx::StencilMode::Always>() && !stencil.mask) {
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

void Context::setColorMode(const gfx::ColorMode& color) {
    if (color.blendFunction.is<gfx::ColorMode::Replace>()) {
        blend = false;
    } else {
        blend = true;
        blendColor = color.blendColor;
        apply_visitor([&] (const auto& blendFunction) {
            blendEquation = gfx::ColorBlendEquationType(blendFunction.equation);
            blendFunc = { blendFunction.srcFactor, blendFunction.dstFactor };
        }, color.blendFunction);
    }

    colorMask = color.mask;
}

std::unique_ptr<gfx::CommandEncoder> Context::createCommandEncoder() {
    backend.updateAssumedState();
    if (backend.contextIsShared()) {
        setDirtyState();
    }
    return std::make_unique<gl::CommandEncoder>(*this);
}

gfx::RenderingStats& Context::renderingStats() {
    return stats;
}

const gfx::RenderingStats& Context::renderingStats() const {
    return stats;
}

void Context::finish() {
    MBGL_CHECK_ERROR(glFinish());
}

void Context::draw(const gfx::DrawMode& drawMode,
                   std::size_t indexOffset,
                   std::size_t indexLength) {
    switch (drawMode.type) {
    case gfx::DrawModeType::Points:
#if not MBGL_USE_GLES2
        // In OpenGL ES 2, the point size is set in the vertex shader.
        pointSize = drawMode.size;
#endif // MBGL_USE_GLES2
        break;
    case gfx::DrawModeType::Lines:
    case gfx::DrawModeType::LineLoop:
    case gfx::DrawModeType::LineStrip:
        lineWidth = drawMode.size;
        break;
    default:
        break;
    }

    MBGL_CHECK_ERROR(glDrawElements(
        Enum<gfx::DrawModeType>::to(drawMode.type),
        static_cast<GLsizei>(indexLength),
        GL_UNSIGNED_SHORT,
        reinterpret_cast<GLvoid*>(sizeof(uint16_t) * indexOffset)));

    stats.numDrawCalls++;
}

void Context::performCleanup() {
    // TODO: Find a better way to unbind VAOs after we're done with them without introducing
    // unnecessary bind(0)/bind(N) sequences.
    {
        activeTextureUnit = 1;
        texture[1] = 0;
        activeTextureUnit = 0;
        texture[0] = 0;

        bindVertexArray = 0;
    }

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
        stats.numBuffers -= int(abandonedBuffers.size());
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
        stats.numCreatedTextures -= int(abandonedTextures.size());
        assert(stats.numCreatedTextures >= 0);
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
        stats.numFrameBuffers -= int(abandonedFramebuffers.size());
        assert(stats.numFrameBuffers >= 0);
        abandonedFramebuffers.clear();
    }

    if (!abandonedRenderbuffers.empty()) {
        MBGL_CHECK_ERROR(glDeleteRenderbuffers(int(abandonedRenderbuffers.size()),
                                               abandonedRenderbuffers.data()));
        abandonedRenderbuffers.clear();
    }
}

void Context::reduceMemoryUsage() {
    performCleanup();

    // Ensure that all pending actions are executed to ensure that they happen before the app goes
    // to the background.
    MBGL_CHECK_ERROR(glFinish());
}

#if not defined(NDEBUG)
void Context::visualizeStencilBuffer() {
#if not MBGL_USE_GLES2
    setStencilMode(gfx::StencilMode::disabled());
    setDepthMode(gfx::DepthMode::disabled());
    setColorMode(gfx::ColorMode::unblended());
    program = 0;

    // Reset the value in case someone else changed it, or it's dirty.
    pixelTransferStencil = gl::value::PixelTransferStencil::Default;

    // Read the stencil buffer
    const auto viewportValue = viewport.getCurrentValue();
    auto image = readFramebuffer<AlphaImage, gfx::TexturePixelType::Stencil>(viewportValue.size, false);

    // Scale the Stencil buffer to cover the entire color space.
    auto it = image.data.get();
    auto end = it + viewportValue.size.width * viewportValue.size.height;
    const auto factor = 255.0f / *std::max_element(it, end);
    for (; it != end; ++it) {
        *it *= factor;
    }

    pixelZoom = { 1, 1 };
    rasterPos = { -1, -1, 0, 1 };
    drawPixels(image);
#endif
}

void Context::visualizeDepthBuffer(const float depthRangeSize) {
    (void)depthRangeSize;
#if not MBGL_USE_GLES2
    setStencilMode(gfx::StencilMode::disabled());
    setDepthMode(gfx::DepthMode::disabled());
    setColorMode(gfx::ColorMode::unblended());
    program = 0;

    // Scales the values in the depth buffer so that they cover the entire grayscale range. This
    // makes it easier to spot tiny differences.
    const float base = 1.0f / (1.0f - depthRangeSize);
    pixelTransferDepth = { base, 1.0f - base };

    // Read the stencil buffer
    auto viewportValue = viewport.getCurrentValue();
    auto image = readFramebuffer<AlphaImage, gfx::TexturePixelType::Depth>(viewportValue.size, false);

    pixelZoom = { 1, 1 };
    rasterPos = { -1, -1, 0, 1 };
    drawPixels(image);
#endif
}

#endif

void Context::clearStencilBuffer(const int32_t bits) {
    MBGL_CHECK_ERROR(glClearStencil(bits));
    MBGL_CHECK_ERROR(glClear(GL_STENCIL_BUFFER_BIT));
}

} // namespace gl
} // namespace mbgl
