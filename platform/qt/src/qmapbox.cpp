#include "qmapbox.hpp"

#include <mbgl/gl/extension.hpp>
#include <mbgl/map/change.hpp>
#include <mbgl/storage/network_status.hpp>
#include <mbgl/util/default_styles.hpp>
#include <mbgl/util/traits.hpp>

#if QT_VERSION >= 0x050000
#include "qquickmapboxgl.hpp"
#include "qquickmapboxglmapparameter.hpp"
#include <QOpenGLContext>
#else
#include <QGLContext>
#endif

// mbgl::NetworkStatus::Status
static_assert(mbgl::underlying_type(QMapbox::Online) == mbgl::underlying_type(mbgl::NetworkStatus::Status::Online), "error");
static_assert(mbgl::underlying_type(QMapbox::Offline) == mbgl::underlying_type(mbgl::NetworkStatus::Status::Offline), "error");

// mbgl::MapChange
static_assert(mbgl::underlying_type(QMapbox::MapChangeRegionWillChange) == mbgl::underlying_type(mbgl::MapChangeRegionWillChange), "error");
static_assert(mbgl::underlying_type(QMapbox::MapChangeRegionWillChangeAnimated) == mbgl::underlying_type(mbgl::MapChangeRegionWillChangeAnimated), "error");
static_assert(mbgl::underlying_type(QMapbox::MapChangeRegionIsChanging) == mbgl::underlying_type(mbgl::MapChangeRegionIsChanging), "error");
static_assert(mbgl::underlying_type(QMapbox::MapChangeRegionDidChange) == mbgl::underlying_type(mbgl::MapChangeRegionDidChange), "error");
static_assert(mbgl::underlying_type(QMapbox::MapChangeRegionDidChangeAnimated) == mbgl::underlying_type(mbgl::MapChangeRegionDidChangeAnimated), "error");
static_assert(mbgl::underlying_type(QMapbox::MapChangeWillStartLoadingMap) == mbgl::underlying_type(mbgl::MapChangeWillStartLoadingMap), "error");
static_assert(mbgl::underlying_type(QMapbox::MapChangeDidFinishLoadingMap) == mbgl::underlying_type(mbgl::MapChangeDidFinishLoadingMap), "error");
static_assert(mbgl::underlying_type(QMapbox::MapChangeDidFailLoadingMap) == mbgl::underlying_type(mbgl::MapChangeDidFailLoadingMap), "error");
static_assert(mbgl::underlying_type(QMapbox::MapChangeWillStartRenderingFrame) == mbgl::underlying_type(mbgl::MapChangeWillStartRenderingFrame), "error");
static_assert(mbgl::underlying_type(QMapbox::MapChangeDidFinishRenderingFrame) == mbgl::underlying_type(mbgl::MapChangeDidFinishRenderingFrame), "error");
static_assert(mbgl::underlying_type(QMapbox::MapChangeDidFinishRenderingFrameFullyRendered) == mbgl::underlying_type(mbgl::MapChangeDidFinishRenderingFrameFullyRendered), "error");
static_assert(mbgl::underlying_type(QMapbox::MapChangeWillStartRenderingMap) == mbgl::underlying_type(mbgl::MapChangeWillStartRenderingMap), "error");
static_assert(mbgl::underlying_type(QMapbox::MapChangeDidFinishRenderingMap) == mbgl::underlying_type(mbgl::MapChangeDidFinishRenderingMap), "error");
static_assert(mbgl::underlying_type(QMapbox::MapChangeDidFinishRenderingMapFullyRendered) == mbgl::underlying_type(mbgl::MapChangeDidFinishRenderingMapFullyRendered), "error");
static_assert(mbgl::underlying_type(QMapbox::MapChangeDidFinishLoadingStyle) == mbgl::underlying_type(mbgl::MapChangeDidFinishLoadingStyle), "error");
static_assert(mbgl::underlying_type(QMapbox::MapChangeSourceDidChange) == mbgl::underlying_type(mbgl::MapChangeSourceDidChange), "error");

namespace QMapbox {

Q_DECL_EXPORT NetworkMode networkMode()
{
    return static_cast<NetworkMode>(mbgl::NetworkStatus::Get());
}

Q_DECL_EXPORT void setNetworkMode(NetworkMode mode)
{
    mbgl::NetworkStatus::Set(static_cast<mbgl::NetworkStatus::Status>(mode));
}

Q_DECL_EXPORT QList<QPair<QString, QString>>& defaultStyles()
{
    static QList<QPair<QString, QString>> styles;

    if (styles.isEmpty()) {
        for (auto style : mbgl::util::default_styles::orderedStyles) {
            styles.append(QPair<QString, QString>(
                QString::fromStdString(style.url), QString::fromStdString(style.name)));
        }
    }

    return styles;
}

Q_DECL_EXPORT void initializeGLExtensions()
{
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

Q_DECL_EXPORT void registerTypes()
{
#if QT_VERSION >= 0x050000
    qmlRegisterType<QQuickMapboxGL>("QQuickMapboxGL", 1, 0, "MapboxMap");
    qmlRegisterType<QQuickMapboxGLMapParameter>("QQuickMapboxGL", 1, 0, "MapParameter");
#endif
}

}
