#include <mbgl/gl/headless_backend.hpp>

#include <QGLWidget>
#include <QOpenGLContext>

#include <cassert>

namespace mbgl {
namespace gl {

class QtBackendImpl final : public HeadlessBackend::Impl {
public:
    ~QtBackendImpl() = default;

    gl::ProcAddress getExtensionFunctionPointer(const char* name) {
        QOpenGLContext* thisContext = QOpenGLContext::currentContext();
        return thisContext->getProcAddress(name);
    }

    void activateContext() {
        widget.makeCurrent();
    }

    void deactivateContext() {
        widget.doneCurrent();
    }

private:
    QGLWidget widget;
};

void HeadlessBackend::createImpl() {
    assert(!impl);
    impl = std::make_unique<QtBackendImpl>();
}

} // namespace gl
} // namespace mbgl
