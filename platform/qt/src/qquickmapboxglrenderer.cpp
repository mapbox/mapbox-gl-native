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
    settings.setViewportMode(QMapboxGLSettings::FlippedYViewport);

    m_map.reset(new QMapboxGL(nullptr, settings));
    connect(m_map.data(), SIGNAL(mapChanged(QMapboxGL::MapChange)), this, SLOT(onMapChanged(QMapboxGL::MapChange)));
}

QQuickMapboxGLRenderer::~QQuickMapboxGLRenderer()
{
}

void QQuickMapboxGLRenderer::onMapChanged(QMapboxGL::MapChange change)
{
    if (change == QMapboxGL::MapChangeDidFinishLoadingMap) {
        m_styleLoaded = true;
        update();
    }
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

    if (syncStatus & QQuickMapboxGL::CenterNeedsSync || syncStatus & QQuickMapboxGL::ZoomNeedsSync) {
        const auto& center = quickMap->center();
        m_map->setCoordinateZoom({ center.latitude(), center.longitude() }, quickMap->zoomLevel());
    }

    if (syncStatus & QQuickMapboxGL::StyleNeedsSync) {
        m_map->setStyleURL(quickMap->style());
        m_styleLoaded = false;
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

    if (m_styleLoaded) {
        if (!quickMap->layoutPropertyChanges().empty()) {
            for (const auto& change: quickMap->layoutPropertyChanges()) {
                m_map->setLayoutProperty(change.layer, change.property, change.value);
            }
            quickMap->layoutPropertyChanges().clear();
        }

        if (!quickMap->paintPropertyChanges().empty()) {
            for (const auto& change: quickMap->paintPropertyChanges()) {
                m_map->setPaintProperty(change.layer, change.property, change.value, change.klass);
            }
            quickMap->paintPropertyChanges().clear();
        }
    }
}
