#include <mbgl/gl/headless_backend.hpp>
#include <mbgl/util/logging.hpp>

#include <GL/osmesa.h>

#include <cassert>

namespace mbgl {

struct OSMesaImpl : public HeadlessBackend::Impl {
    OSMesaImpl(OSMesaContext glContext_) : glContext(glContext_) {
    }

    ~OSMesaImpl() {
        OSMesaDestroyContext(glContext);
    }

    void activateContext() final {
        if (!OSMesaMakeCurrent(glContext, &fakeBuffer, GL_UNSIGNED_BYTE, 1, 1)) {
            throw std::runtime_error("Switching OpenGL context failed.\n");
        }
    }

    OSMesaContext glContext = nullptr;
    GLubyte fakeBuffer = 0;
};

gl::ProcAddress HeadlessBackend::initializeExtension(const char* name) {
    return OSMesaGetProcAddress(name);
}

bool HeadlessBackend::hasDisplay() {
    return true;
};

void HeadlessBackend::createContext() {
    assert(!hasContext());

#if OSMESA_MAJOR_VERSION * 100 + OSMESA_MINOR_VERSION >= 305
    OSMesaContext glContext = OSMesaCreateContextExt(OSMESA_RGBA, 16, 0, 0, nullptr);
#else
    OSMesaContext glContext = OSMesaCreateContext(OSMESA_RGBA, nullptr);
#endif
    if (glContext == nullptr) {
        throw std::runtime_error("Error creating GL context object.");
    }

    impl.reset(new OSMesaImpl(glContext));
}

} // namespace mbgl
