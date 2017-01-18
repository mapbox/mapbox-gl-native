#include <mbgl/gl/headless_backend.hpp>

#include <QGLContext>
#include <QGLWidget>

#if QT_VERSION >= 0x050000
#include <QOpenGLContext>
#endif

#include <cassert>

namespace mbgl {

struct QtImpl : public HeadlessBackend::Impl {
    void activateContext() final {
        widget.makeCurrent();
    }

    void deactivateContext() final {
        widget.doneCurrent();
    }

    QGLWidget widget;
};

gl::glProc HeadlessBackend::initializeExtension(const char* name) {
#if QT_VERSION >= 0x050000
        QOpenGLContext* thisContext = QOpenGLContext::currentContext();
        return thisContext->getProcAddress(name);
#else
        const QGLContext* thisContext = QGLContext::currentContext();
        return reinterpret_cast<mbgl::gl::glProc>(thisContext->getProcAddress(name));
#endif
}

bool HeadlessBackend::hasDisplay() {
    return true;
};

void HeadlessBackend::createContext() {
    assert(!hasContext());
    impl.reset(new QtImpl);
}

} // namespace mbgl
