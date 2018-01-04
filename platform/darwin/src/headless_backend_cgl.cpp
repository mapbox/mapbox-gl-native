#include <mbgl/gl/headless_backend.hpp>
#include <mbgl/util/logging.hpp>

#include <OpenGL/OpenGL.h>
#include <CoreFoundation/CoreFoundation.h>

#include <string>
#include <stdexcept>

namespace mbgl {

// This class provides a singleton that contains information about the pixel format used for
// instantiating new headless rendering contexts.
class CGLDisplayConfig {
private:
    // Key for singleton construction.
    struct Key { explicit Key() = default; };

public:
    CGLDisplayConfig(Key) {
        // TODO: test if OpenGL 4.1 with GL_ARB_ES2_compatibility is supported
        // If it is, use kCGLOGLPVersion_3_2_Core and enable that extension.
        CGLPixelFormatAttribute attributes[] = {
            kCGLPFAOpenGLProfile, static_cast<CGLPixelFormatAttribute>(kCGLOGLPVersion_Legacy),
            // Not adding kCGLPFAAccelerated, as this will make headless rendering impossible when running in VMs.
            kCGLPFAClosestPolicy,
            kCGLPFAAccumSize, static_cast<CGLPixelFormatAttribute>(32),
            kCGLPFAColorSize, static_cast<CGLPixelFormatAttribute>(24),
            kCGLPFAAlphaSize, static_cast<CGLPixelFormatAttribute>(8),
            kCGLPFADepthSize, static_cast<CGLPixelFormatAttribute>(16),
            kCGLPFAStencilSize, static_cast<CGLPixelFormatAttribute>(8),
            kCGLPFASupportsAutomaticGraphicsSwitching,
            kCGLPFAAllowOfflineRenderers, // Allows using the integrated GPU
            static_cast<CGLPixelFormatAttribute>(0)
        };

        GLint num;
        // TODO: obtain all configurations and choose the best one.
        const CGLError error = CGLChoosePixelFormat(attributes, &pixelFormat, &num);
        if (error != kCGLNoError) {
            throw std::runtime_error(std::string("Error choosing pixel format:") + CGLErrorString(error) + "\n");
        }
        if (num <= 0) {
            throw std::runtime_error("No pixel formats found.");
        }
    }

    ~CGLDisplayConfig() {
        const CGLError error = CGLDestroyPixelFormat(pixelFormat);
        if (error != kCGLNoError) {
            Log::Error(Event::OpenGL, std::string("Error destroying pixel format:") + CGLErrorString(error));
        }
    }

    static std::shared_ptr<const CGLDisplayConfig> create() {
        static std::weak_ptr<const CGLDisplayConfig> instance;
        auto shared = instance.lock();
        if (!shared) {
            instance = shared = std::make_shared<CGLDisplayConfig>(Key{});
        }
        return shared;
    }

public:
    CGLPixelFormatObj pixelFormat = nullptr;
};

class CGLBackendImpl : public HeadlessBackend::Impl {
public:
    CGLBackendImpl() {
        CGLError error = CGLCreateContext(cglDisplay->pixelFormat, nullptr, &glContext);
        if (error != kCGLNoError) {
            throw std::runtime_error(std::string("Error creating GL context object:") +
                                     CGLErrorString(error) + "\n");
        }

        error = CGLEnable(glContext, kCGLCEMPEngine);
        if (error != kCGLNoError) {
            throw std::runtime_error(std::string("Error enabling OpenGL multithreading:") +
                                     CGLErrorString(error) + "\n");
        }
    }

    ~CGLBackendImpl() final {
        CGLDestroyContext(glContext);
    }

    gl::ProcAddress getExtensionFunctionPointer(const char* name) final {
        static CFBundleRef framework = CFBundleGetBundleWithIdentifier(CFSTR("com.apple.opengl"));
        if (!framework) {
            throw std::runtime_error("Failed to load OpenGL framework.");
        }

        CFStringRef str =
            CFStringCreateWithCString(kCFAllocatorDefault, name, kCFStringEncodingASCII);
        void* symbol = CFBundleGetFunctionPointerForName(framework, str);
        CFRelease(str);

        return reinterpret_cast<gl::ProcAddress>(symbol);
    }

    void activateContext() final {
        CGLError error = CGLSetCurrentContext(glContext);
        if (error != kCGLNoError) {
            throw std::runtime_error(std::string("Switching OpenGL context failed:") +
                                     CGLErrorString(error) + "\n");
        }
    }

    void deactivateContext() final {
        CGLError error = CGLSetCurrentContext(nullptr);
        if (error != kCGLNoError) {
            throw std::runtime_error(std::string("Removing OpenGL context failed:") +
                                     CGLErrorString(error) + "\n");
        }
    }

private:
    const std::shared_ptr<const CGLDisplayConfig> cglDisplay = CGLDisplayConfig::create();
    CGLContextObj glContext = nullptr;
};

void HeadlessBackend::createImpl() {
    assert(!impl);
    impl = std::make_unique<CGLBackendImpl>();
}

} // namespace mbgl
