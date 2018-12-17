#include <mbgl/gl/headless_backend.hpp>

#include <QGLWidget>
#include <QOpenGLContext>

#include <cassert>

namespace mbgl {

class QtBackendImpl : public HeadlessBackend::Impl {
public:
    ~QtBackendImpl() final = default;

    gl::ProcAddress getExtensionFunctionPointer(const char* name) final {
        QOpenGLContext* thisContext = QOpenGLContext::currentContext();
        return thisContext->getProcAddress(name);
    }

    void activateContext() final {
        widget.makeCurrent();
    }

    void deactivateContext() final {
        widget.doneCurrent();
    }

private:
    QGLWidget widget;
};

void HeadlessBackend::createImpl() {
    assert(!impl);
    impl = std::make_unique<QtBackendImpl>();
}

} // namespace mbgl
