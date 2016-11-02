#include <mbgl/platform/default/headless_backend.hpp>
#include <mbgl/platform/log.hpp>

namespace mbgl {

gl::glProc HeadlessBackend::initializeExtension(const char* name) {
    return OSMesaGetProcAddress(name);
}

void HeadlessBackend::createContext() {
    if (glContext == nullptr) {
#if OSMESA_MAJOR_VERSION * 100 + OSMESA_MINOR_VERSION >= 305
        glContext = OSMesaCreateContextExt(OSMESA_RGBA, 16, 0, 0, nullptr);
#else
        glContext = OSMesaCreateContext(OSMESA_RGBA, nullptr);
#endif
        if (glContext == nullptr) {
            Log::Error(Event::OpenGL, "failed to create OSMesa context");
        }
    }

    if (glContext == nullptr) {
        throw std::runtime_error("Error creating GL context object.");
    }
}

void HeadlessBackend::destroyContext() {
    if (glContext) {
        if (glContext != OSMesaGetCurrentContext()) {
            activateContext();
        }
        OSMesaDestroyContext(glContext);
        glContext = nullptr;
    }
}

void HeadlessBackend::activateContext() {
    if (!OSMesaMakeCurrent(glContext, &fakeBuffer, GL_UNSIGNED_BYTE, 1, 1)) {
        throw std::runtime_error("Switching OpenGL context failed.\n");
    }
}

void HeadlessBackend::deactivateContext() {
    // no-op.
}

} // namespace mbgl
