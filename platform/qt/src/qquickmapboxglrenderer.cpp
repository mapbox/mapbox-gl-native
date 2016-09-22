#include "qquickmapboxglrenderer.hpp"

#include <QMapboxGL>
#include <QQuickMapboxGL>
#include <QQuickMapboxGLStyle>

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
    settings.setViewportMode(QMapboxGLSettings::FlippedYViewport);

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
    auto quickMap = qobject_cast<QQuickMapboxGL*>(item);
    if (!m_initialized) {
        QObject::connect(m_map.data(), &QMapboxGL::needsRendering, quickMap, &QQuickMapboxGL::update);
        QObject::connect(m_map.data(), SIGNAL(mapChanged(QMapbox::MapChange)), quickMap, SLOT(onMapChanged(QMapbox::MapChange)));
        QObject::connect(this, &QQuickMapboxGLRenderer::centerChanged, quickMap, &QQuickMapboxGL::setCenter);
        m_initialized = true;
    }

    auto syncStatus = quickMap->m_syncState;
    quickMap->m_syncState = QQuickMapboxGL::NothingNeedsSync;

    if (syncStatus & QQuickMapboxGL::CenterNeedsSync || syncStatus & QQuickMapboxGL::ZoomNeedsSync) {
        const auto& center = quickMap->center();
        m_map->setCoordinateZoom({ center.latitude(), center.longitude() }, quickMap->zoomLevel());
    }

    if (syncStatus & QQuickMapboxGL::StyleNeedsSync && quickMap->style()) {
        m_map->setStyleUrl(quickMap->style()->url());
    }

    if (syncStatus & QQuickMapboxGL::PanNeedsSync) {
        m_map->moveBy(quickMap->m_pan);
        quickMap->m_pan = QPointF();
        emit centerChanged(QGeoCoordinate(m_map->latitude(), m_map->longitude()));
    }

    if (syncStatus & QQuickMapboxGL::BearingNeedsSync) {
        m_map->setBearing(quickMap->bearing());
    }

    if (syncStatus & QQuickMapboxGL::PitchNeedsSync) {
        m_map->setPitch(quickMap->pitch());
    }

    if (!quickMap->m_styleLoaded) {
        return;
    }

    for (const auto& change: quickMap->m_layoutChanges) {
        m_map->setLayoutProperty(change.value("layer").toString(), change.value("property").toString(), change.value("value"));
    }
    quickMap->m_layoutChanges.clear();

    for (const auto& change: quickMap->m_paintChanges) {
        m_map->setPaintProperty(change.value("layer").toString(), change.value("property").toString(), change.value("value"), change.value("class").toString());
    }
    quickMap->m_paintChanges.clear();
}
