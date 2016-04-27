#ifndef QQUICKMAPBOXGL_H
#define QQUICKMAPBOXGL_H

#include <QGeoCoordinate>
#include <QGeoShape>
#include <QQuickFramebufferObject>

class QQuickItem;

class Q_DECL_EXPORT QQuickMapboxGL : public QQuickFramebufferObject
{
    Q_OBJECT

    Q_PROPERTY(qreal minimumZoomLevel READ minimumZoomLevel WRITE setMinimumZoomLevel NOTIFY minimumZoomLevelChanged)
    Q_PROPERTY(qreal maximumZoomLevel READ maximumZoomLevel WRITE setMaximumZoomLevel NOTIFY maximumZoomLevelChanged)
    Q_PROPERTY(qreal zoomLevel READ zoomLevel WRITE setZoomLevel NOTIFY zoomLevelChanged)
    Q_PROPERTY(QGeoCoordinate center READ center WRITE setCenter NOTIFY centerChanged)
    Q_PROPERTY(QGeoShape visibleRegion READ visibleRegion WRITE setVisibleRegion)

public:
    QQuickMapboxGL(QQuickItem *parent = 0);
    virtual ~QQuickMapboxGL();

    // QQuickFramebufferObject implementation.
    virtual Renderer *createRenderer() const Q_DECL_FINAL;

    void setMinimumZoomLevel(qreal minimumZoomLevel);
    qreal minimumZoomLevel() const;

    void setMaximumZoomLevel(qreal maximumZoomLevel);
    qreal maximumZoomLevel() const;

    void setZoomLevel(qreal zoomLevel);
    qreal zoomLevel() const;

    void setCenter(const QGeoCoordinate &center);
    QGeoCoordinate center() const;

    void setVisibleRegion(const QGeoShape &shape);
    QGeoShape visibleRegion() const;

    enum SyncState {
        NothingNeedsSync = 0x00,
        ZoomNeedsSync    = 0x01,
        CenterNeedsSync  = 0x02,
    };

    int swapSyncState();

signals:
    void minimumZoomLevelChanged();
    void maximumZoomLevelChanged();
    void zoomLevelChanged(qreal zoomLevel);
    void centerChanged(const QGeoCoordinate &coordinate);

private:
    qreal m_minimumZoomLevel = 0;
    qreal m_maximumZoomLevel = 20;
    qreal m_zoomLevel = 20;

    QGeoCoordinate m_center;
    QGeoShape m_visibleRegion;

    int m_syncState = NothingNeedsSync;
};

#endif // QQUICKMAPBOXGL_H
