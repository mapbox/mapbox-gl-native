#include "qquickmapboxglrenderer.hpp"

#include <mbgl/util/constants.hpp>

#include <QQuickMapboxGL>

#include <QDebug>
#include <QQuickItem>
#include <QString>
#include <QtGlobal>

#include <cmath>

QQuickMapboxGL::QQuickMapboxGL(QQuickItem *parent_)
    : QQuickFramebufferObject(parent_)
{
}

QQuickMapboxGL::~QQuickMapboxGL()
{
}

QQuickFramebufferObject::Renderer *QQuickMapboxGL::createRenderer() const
{
    return new QQuickMapboxGLRenderer();
}

void QQuickMapboxGL::setPlugin(QDeclarativeGeoServiceProvider *)
{
    qWarning() << __PRETTY_FUNCTION__
        << "Not implemented.";
}

QDeclarativeGeoServiceProvider *QQuickMapboxGL::plugin() const
{
    return nullptr;
}

void QQuickMapboxGL::setMinimumZoomLevel(qreal zoom)
{
    zoom = qMax(mbgl::util::MIN_ZOOM, zoom);
    zoom = qMin(m_maximumZoomLevel, zoom);

    if (m_minimumZoomLevel == zoom) {
        return;
    }

    m_minimumZoomLevel = zoom;
    setZoomLevel(m_zoomLevel); // Constrain.

    emit minimumZoomLevelChanged();
}

qreal QQuickMapboxGL::minimumZoomLevel() const
{
    return m_minimumZoomLevel;
}

void QQuickMapboxGL::setMaximumZoomLevel(qreal zoom)
{
    zoom = qMin(mbgl::util::MAX_ZOOM, zoom);
    zoom = qMax(m_minimumZoomLevel, zoom);

    if (m_maximumZoomLevel == zoom) {
        return;
    }

    m_maximumZoomLevel = zoom;
    setZoomLevel(m_zoomLevel); // Constrain.

    emit maximumZoomLevelChanged();
}

qreal QQuickMapboxGL::maximumZoomLevel() const
{
    return m_maximumZoomLevel;
}

void QQuickMapboxGL::setZoomLevel(qreal zoom)
{
    zoom = qMin(m_maximumZoomLevel, zoom);
    zoom = qMax(m_minimumZoomLevel, zoom);

    if (m_zoomLevel == zoom) {
        return;
    }

    m_zoomLevel = zoom;

    m_syncState |= ZoomNeedsSync;
    update();

    emit zoomLevelChanged(m_zoomLevel);
}

qreal QQuickMapboxGL::zoomLevel() const
{
    return m_zoomLevel;
}

void QQuickMapboxGL::setCenter(const QGeoCoordinate &coordinate)
{
    if (m_center == coordinate) {
        return;
    }

    m_center = coordinate;

    m_syncState |= CenterNeedsSync;
    update();

    emit centerChanged(m_center);
}

QGeoCoordinate QQuickMapboxGL::center() const
{
    return m_center;
}

QGeoServiceProvider::Error QQuickMapboxGL::error() const
{
    return QGeoServiceProvider::NoError;
}

QString QQuickMapboxGL::errorString() const
{
    return QString();
}

void QQuickMapboxGL::setVisibleRegion(const QGeoShape &shape)
{
    m_visibleRegion = shape;
}

QGeoShape QQuickMapboxGL::visibleRegion() const
{
    return m_visibleRegion;
}

void QQuickMapboxGL::setCopyrightsVisible(bool)
{
    qWarning() << __PRETTY_FUNCTION__
        << "Not implemented.";
}

bool QQuickMapboxGL::copyrightsVisible() const
{
    return false;
}

void QQuickMapboxGL::setColor(const QColor &color)
{
    if (m_color == color) {
        return;
    }

    m_color = color;

    setPaintProperty("background", "background-color", color);

    emit colorChanged(m_color);
}

QColor QQuickMapboxGL::color() const
{
    return m_color;
}

void QQuickMapboxGL::pan(int dx, int dy)
{
    m_pan += QPointF(dx, -dy);

    m_syncState |= PanNeedsSync;
    update();
}

void QQuickMapboxGL::setLayoutProperty(const QString &layer, const QString &property, const QVariant &value)
{
    m_layoutChanges.append(LayoutPropertyChange { layer, property, value });
    update();
}

void QQuickMapboxGL::setPaintProperty(const QString &layer, const QString &property, const QVariant &value, const QString &klass)
{
    m_paintChanges.append(PaintPropertyChange { layer, property, value, klass });
    update();
}

void QQuickMapboxGL::setStyle(const QString &styleUrl)
{
    if (m_style == styleUrl) {
        return;
    }

    m_style = styleUrl;

    m_syncState |= StyleNeedsSync;
    update();

    emit styleChanged();
}

QString QQuickMapboxGL::style() const
{
    return m_style;
}

void QQuickMapboxGL::setBearing(qreal angle)
{
    angle = std::fmod(angle, 360.);

    if (m_bearing == angle) {
        return;
    }

    m_bearing = angle;

    m_syncState |= BearingNeedsSync;
    update();

    emit bearingChanged(m_bearing);
}

qreal QQuickMapboxGL::bearing() const
{
    return m_bearing;
}

void QQuickMapboxGL::setPitch(qreal angle)
{
    angle = qMin(qMax(0., angle), mbgl::util::PITCH_MAX * mbgl::util::RAD2DEG);

    if (m_pitch == angle) {
        return;
    }

    m_pitch = angle;

    m_syncState |= PitchNeedsSync;
    update();

    emit pitchChanged(m_pitch);
}

qreal QQuickMapboxGL::pitch() const
{
    return m_pitch;
}

QPointF QQuickMapboxGL::swapPan()
{
    QPointF oldPan = m_pan;

    m_pan = QPointF(0, 0);

    return oldPan;
}

int QQuickMapboxGL::swapSyncState()
{
    int oldState = m_syncState;

    m_syncState = NothingNeedsSync;

    return oldState;
}
