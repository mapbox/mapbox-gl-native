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
    connect(m_map.data(), SIGNAL(mapChanged(QMapbox::MapChange)), this, SLOT(onMapChanged(QMapbox::MapChange)));
}

QQuickMapboxGLRenderer::~QQuickMapboxGLRenderer()
{
}

void QQuickMapboxGLRenderer::onMapChanged(QMapbox::MapChange change)
{
    auto onMapChangeWillStartLoadingMap = [&]() {
        m_styleLoaded = false;
    };

    auto onMapChangeDidFinishLoadingMap = [&]() {
        m_styleLoaded = true;
        emit styleChanged();
    };

    switch (change) {
    case QMapbox::MapChangeWillStartLoadingMap:
        onMapChangeWillStartLoadingMap();
        break;
    case QMapbox::MapChangeDidFinishLoadingMap:
        onMapChangeDidFinishLoadingMap();
        break;
    default:
        break;
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

    if (syncStatus & QQuickMapboxGL::StyleNeedsSync && quickMap->style()) {
        m_map->setStyleUrl(quickMap->style()->url());
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
                m_map->setLayoutProperty(change.value("layer").toString(), change.value("property").toString(), change.value("value"));
            }
            quickMap->layoutPropertyChanges().clear();
        }

        if (!quickMap->paintPropertyChanges().empty()) {
            for (const auto& change: quickMap->paintPropertyChanges()) {
                m_map->setPaintProperty(change.value("layer").toString(), change.value("property").toString(), change.value("value"), change.value("class").toString());
            }
            quickMap->paintPropertyChanges().clear();
        }
    }
}
