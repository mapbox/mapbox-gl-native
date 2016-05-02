#include "qquickmapboxglrenderer.hpp"

#include <QMapboxGL>
#include <QQuickMapboxGL>

#include <QSize>
#include <QOpenGLFramebufferObject>
#include <QOpenGLFramebufferObjectFormat>
#include <QQuickWindow>

QQuickMapboxGLRenderer::QQuickMapboxGLRenderer()
{
    QMapbox::initializeGLExtensions();

    QMapboxGLSettings settings;
    settings.setAccessToken(qgetenv("MAPBOX_ACCESS_TOKEN"));
    settings.setCacheDatabasePath("/tmp/mbgl-cache.db");
    settings.setCacheDatabaseMaximumSize(20 * 1024 * 1024);

    m_map.reset(new QMapboxGL(nullptr, settings));
}

QQuickMapboxGLRenderer::~QQuickMapboxGLRenderer()
{
}

QOpenGLFramebufferObject* QQuickMapboxGLRenderer::createFramebufferObject(const QSize &size)
{
    m_map->resize(size);

    QOpenGLFramebufferObjectFormat format;
    format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);

    return new QOpenGLFramebufferObject(size, format);
}

void QQuickMapboxGLRenderer::render()
{
    m_map->render();
}

void QQuickMapboxGLRenderer::synchronize(QQuickFramebufferObject *item)
{
    if (!m_initialized) {
        auto qquickMapbox = static_cast<QQuickMapboxGL*>(item);

        QObject::connect(m_map.data(), &QMapboxGL::needsRendering, qquickMapbox, &QQuickMapboxGL::update);
        QObject::connect(this, &QQuickMapboxGLRenderer::centerChanged, qquickMapbox, &QQuickMapboxGL::setCenter);

        m_initialized = true;
    }

    auto quickMap = static_cast<QQuickMapboxGL*>(item);
    auto syncStatus = quickMap->swapSyncState();

    if (syncStatus & QQuickMapboxGL::ZoomNeedsSync) {
        m_map->setZoom(quickMap->zoomLevel());
    }

    if (syncStatus & QQuickMapboxGL::CenterNeedsSync) {
        const auto& center = quickMap->center();
        m_map->setCoordinate(QMapbox::Coordinate(center.latitude(), center.longitude()));
    }

    if (syncStatus & QQuickMapboxGL::StyleNeedsSync) {
        m_map->setStyleURL(quickMap->style());
    }

    if (syncStatus & QQuickMapboxGL::PanNeedsSync) {
        m_map->moveBy(quickMap->swapPan());
        emit centerChanged(QGeoCoordinate(m_map->latitude(), m_map->longitude()));
    }

    if (syncStatus & QQuickMapboxGL::BearingNeedsSync) {
        m_map->setBearing(quickMap->bearing());
    }

    if (syncStatus & QQuickMapboxGL::PitchNeedsSync) {
        m_map->setPitch(quickMap->pitch());
    }
}
