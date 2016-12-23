#include "qquickmapboxglrenderer.hpp"

#include "qmapboxgl.hpp"
#include "qquickmapboxgl.hpp"

#include <QSize>
#include <QOpenGLFramebufferObject>
#include <QOpenGLFramebufferObjectFormat>
#include <QQuickWindow>
#include <QThread>

QQuickMapboxGLRenderer::QQuickMapboxGLRenderer()
{
    QMapbox::initializeGLExtensions();

    QMapboxGLSettings settings;
    settings.setAccessToken(qgetenv("MAPBOX_ACCESS_TOKEN"));
    settings.setCacheDatabasePath("/tmp/mbgl-cache.db");
    settings.setCacheDatabaseMaximumSize(20 * 1024 * 1024);
    settings.setViewportMode(QMapboxGLSettings::FlippedYViewport);

    m_map.reset(new QMapboxGL(nullptr, settings, QSize(256, 256), 1));
}

QQuickMapboxGLRenderer::~QQuickMapboxGLRenderer()
{
}

QOpenGLFramebufferObject* QQuickMapboxGLRenderer::createFramebufferObject(const QSize &size)
{
    m_map->resize(size / m_pixelRatio, size);

    QOpenGLFramebufferObjectFormat format;
    format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);

    return new QOpenGLFramebufferObject(size, format);
}

void QQuickMapboxGLRenderer::render()
{
    m_map->render(framebufferObject());
}

void QQuickMapboxGLRenderer::synchronize(QQuickFramebufferObject *item)
{
    auto quickMap = qobject_cast<QQuickMapboxGL*>(item);
    if (!m_initialized) {
        QObject::connect(m_map.data(), &QMapboxGL::needsRendering, quickMap, &QQuickMapboxGL::update);
        QObject::connect(m_map.data(), SIGNAL(mapChanged(QMapboxGL::MapChange)), quickMap, SLOT(onMapChanged(QMapboxGL::MapChange)));
        QObject::connect(this, &QQuickMapboxGLRenderer::centerChanged, quickMap, &QQuickMapboxGL::setCenter);
        m_initialized = true;
    }

    if (auto window = quickMap->window()) {
        m_pixelRatio = window->devicePixelRatio();
    } else {
        m_pixelRatio = 1;
    }

    auto syncStatus = quickMap->m_syncState;
    quickMap->m_syncState = QQuickMapboxGL::NothingNeedsSync;

    if (syncStatus & QQuickMapboxGL::CenterNeedsSync || syncStatus & QQuickMapboxGL::ZoomNeedsSync) {
        const auto& center = quickMap->center();
        m_map->setCoordinateZoom({ center.latitude(), center.longitude() }, quickMap->zoomLevel());
    }

    if (syncStatus & QQuickMapboxGL::StyleNeedsSync && !quickMap->m_styleUrl.isEmpty()) {
        m_map->setStyleUrl(quickMap->m_styleUrl);
    }

    if (syncStatus & QQuickMapboxGL::PanNeedsSync) {
        m_map->moveBy(quickMap->m_pan);
        quickMap->m_pan = QPointF();
        emit centerChanged(QGeoCoordinate(m_map->latitude(), m_map->longitude()));
    }

    if (syncStatus & QQuickMapboxGL::BearingNeedsSync) {
        m_map->setBearing(quickMap->m_bearing);
    }

    if (syncStatus & QQuickMapboxGL::PitchNeedsSync) {
        m_map->setPitch(quickMap->m_pitch);
    }

    if (!quickMap->m_styleLoaded) {
        return;
    }

    for (const auto& change : quickMap->m_sourceChanges) {
        m_map->updateSource(change.value("id").toString(), change);
    }
    quickMap->m_sourceChanges.clear();

    for (const auto& change : quickMap->m_layerChanges) {
        m_map->addLayer(change);
    }
    quickMap->m_layerChanges.clear();

    for (const auto& change : quickMap->m_filterChanges) {
        m_map->setFilter(change.value("layer").toString(), change.value("filter"));
    }
    quickMap->m_filterChanges.clear();

    for (const auto& change : quickMap->m_imageChanges) {
        m_map->addImage(change.name, change.sprite);
    }
    quickMap->m_imageChanges.clear();

    for (const auto& change : quickMap->m_stylePropertyChanges) {
        if (change.type == QQuickMapboxGL::StyleProperty::Paint) {
            m_map->setPaintProperty(change.layer, change.property, change.value, change.klass);
        } else {
            m_map->setLayoutProperty(change.layer, change.property, change.value);
        }
    }
    quickMap->m_stylePropertyChanges.clear();
}
