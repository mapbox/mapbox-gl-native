#include <mbgl/gl/gl.hpp>
#include <mbgl/storage/network_status.hpp>
#include <mbgl/util/default_styles.hpp>
#include <mbgl/util/traits.hpp>

#include <QMapbox>

#if QT_VERSION >= 0x050000
#include <QOpenGLContext>
#else
#include <QGLContext>
#endif

// mbgl::MapMode
static_assert(mbgl::underlying_type(QMapbox::Online) ==
                  mbgl::underlying_type(mbgl::NetworkStatus::Status::Online),
              "error");
static_assert(mbgl::underlying_type(QMapbox::Offline) ==
                  mbgl::underlying_type(mbgl::NetworkStatus::Status::Offline),
              "error");

namespace QMapbox {

Q_DECL_EXPORT NetworkMode networkMode() {
    return static_cast<NetworkMode>(mbgl::NetworkStatus::Get());
}

Q_DECL_EXPORT void setNetworkMode(NetworkMode mode) {
    mbgl::NetworkStatus::Set(static_cast<mbgl::NetworkStatus::Status>(mode));
}

Q_DECL_EXPORT QList<QPair<QString, QString>>& defaultStyles() {
    static QList<QPair<QString, QString>> styles;

    if (styles.isEmpty()) {
        for (auto style : mbgl::util::default_styles::orderedStyles) {
            styles.append(QPair<QString, QString>(QString::fromStdString(style.url),
                                                  QString::fromStdString(style.name)));
        }
    }

    return styles;
}

Q_DECL_EXPORT void initializeGLExtensions() {
    mbgl::gl::InitializeExtensions([](const char* name) {
#if QT_VERSION >= 0x050000
        QOpenGLContext* thisContext = QOpenGLContext::currentContext();
        return thisContext->getProcAddress(name);
#else
        const QGLContext* thisContext = QGLContext::currentContext();
        return reinterpret_cast<mbgl::gl::glProc>(thisContext->getProcAddress(name));
#endif
    });
}
}
