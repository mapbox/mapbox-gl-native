#include <mbgl/gl/headless_backend.hpp>
#include <mbgl/util/logging.hpp>

#include <GL/osmesa.h>

#include <cassert>

namespace mbgl {

class OSMesaBackendImpl : public HeadlessBackend::Impl {
public:
    OSMesaBackendImpl() {
#if OSMESA_MAJOR_VERSION * 100 + OSMESA_MINOR_VERSION >= 305
        glContext = OSMesaCreateContextExt(OSMESA_RGBA, 16, 0, 0, nullptr);
#else
        glContext = OSMesaCreateContext(OSMESA_RGBA, nullptr);
#endif
        if (glContext == nullptr) {
            throw std::runtime_error("Error creating GL context object.");
        }
    }

    ~OSMesaBackendImpl() final {
        OSMesaDestroyContext(glContext);
    }

    gl::ProcAddress getExtensionFunctionPointer(const char* name) final {
        return OSMesaGetProcAddress(name);
    }

    void activateContext() final {
        if (!OSMesaMakeCurrent(glContext, &fakeBuffer, GL_UNSIGNED_BYTE, 1, 1)) {
            throw std::runtime_error("Switching OpenGL context failed.\n");
        }
    }

private:
    OSMesaContext glContext = nullptr;
    GLubyte fakeBuffer = 0;
};

void HeadlessBackend::createImpl() {
    assert(!impl);
    impl = std::make_unique<OSMesaBackendImpl>();
}

} // namespace mbgl
