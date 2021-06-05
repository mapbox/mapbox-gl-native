#include <mbgl/gl/headless_backend.hpp>

#include <QOffscreenSurface>
#include <QOpenGLContext>

#include <cassert>

namespace mbgl {
namespace gl {

class QtBackendImpl final : public HeadlessBackend::Impl {
public:
    QtBackendImpl() {
        // QtBackendImpl must be created in the main/GUI thread on platforms
        // that have a QWindow-based QOffscreenSurface.
        surface.create();
        context.create();
    }
    ~QtBackendImpl() = default;

    gl::ProcAddress getExtensionFunctionPointer(const char* name) {
        QOpenGLContext* thisContext = QOpenGLContext::currentContext();
        return thisContext->getProcAddress(name);
    }

    void activateContext() {
        context.makeCurrent(&surface);
    }

    void deactivateContext() {
        context.doneCurrent();
    }

private:
    QOpenGLContext context;
    QOffscreenSurface surface;
};

void HeadlessBackend::createImpl() {
    assert(!impl);
    impl = std::make_unique<QtBackendImpl>();
}

} // namespace gl
} // namespace mbgl
