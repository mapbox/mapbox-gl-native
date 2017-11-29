#include <mbgl/gl/headless_backend.hpp>

#include <QGLWidget>

#if QT_VERSION >= 0x050000
#include <QOpenGLContext>
#else
#include <QGLContext>
#endif

#include <cassert>

namespace mbgl {

class QtBackendImpl : public HeadlessBackend::Impl {
public:
    ~QtBackendImpl() final = default;

    gl::ProcAddress getExtensionFunctionPointer(const char* name) final {
#if QT_VERSION >= 0x050000
        QOpenGLContext* thisContext = QOpenGLContext::currentContext();
        return thisContext->getProcAddress(name);
#else
        const QGLContext* thisContext = QGLContext::currentContext();
        return reinterpret_cast<gl::ProcAddress>(thisContext->getProcAddress(name));
#endif
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
