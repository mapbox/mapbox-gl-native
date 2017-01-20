#include <mbgl/platform/default/headless_backend.hpp>
#include <mbgl/platform/default/headless_display.hpp>

#include <QApplication>
#include <QGLContext>
#include <QGLWidget>

#if QT_VERSION >= 0x050000
#include <QOpenGLContext>
#endif

namespace mbgl {

gl::glProc HeadlessBackend::initializeExtension(const char* name) {
#if QT_VERSION >= 0x050000
        QOpenGLContext* thisContext = QOpenGLContext::currentContext();
        return thisContext->getProcAddress(name);
#else
        const QGLContext* thisContext = QGLContext::currentContext();
        return reinterpret_cast<mbgl::gl::glProc>(thisContext->getProcAddress(name));
#endif
}

void HeadlessBackend::createContext() {
    static const char* argv[] = { "mbgl" };
    static int argc = 1;
    static auto* app = new QApplication(argc, const_cast<char**>(argv));

    Q_UNUSED(app);

    glContext = new QGLWidget;
}

void HeadlessBackend::destroyContext() {
    delete glContext;
}

void HeadlessBackend::activateContext() {
    glContext->makeCurrent();
}

void HeadlessBackend::deactivateContext() {
    glContext->doneCurrent();
}

} // namespace mbgl
