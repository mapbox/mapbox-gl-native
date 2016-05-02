#ifndef QQUICKMAPBOXGL_H
#define QQUICKMAPBOXGL_H

#include <QGeoCoordinate>
#include <QGeoServiceProvider>
#include <QGeoShape>
#include <QPointF>
#include <QQuickFramebufferObject>

class QDeclarativeGeoServiceProvider;
class QQuickItem;

class Q_DECL_EXPORT QQuickMapboxGL : public QQuickFramebufferObject
{
    Q_OBJECT

    // Map QML Type interface implementation.
    Q_ENUMS(QGeoServiceProvider::Error)
    Q_PROPERTY(QDeclarativeGeoServiceProvider *plugin READ plugin WRITE setPlugin NOTIFY pluginChanged)
    Q_PROPERTY(qreal minimumZoomLevel READ minimumZoomLevel WRITE setMinimumZoomLevel NOTIFY minimumZoomLevelChanged)
    Q_PROPERTY(qreal maximumZoomLevel READ maximumZoomLevel WRITE setMaximumZoomLevel NOTIFY maximumZoomLevelChanged)
    Q_PROPERTY(qreal zoomLevel READ zoomLevel WRITE setZoomLevel NOTIFY zoomLevelChanged)
    Q_PROPERTY(QGeoCoordinate center READ center WRITE setCenter NOTIFY centerChanged)
    Q_PROPERTY(QGeoServiceProvider::Error error READ error NOTIFY errorChanged)
    Q_PROPERTY(QString errorString READ errorString NOTIFY errorChanged)
    Q_PROPERTY(QGeoShape visibleRegion READ visibleRegion WRITE setVisibleRegion)
    Q_PROPERTY(bool copyrightsVisible READ copyrightsVisible WRITE setCopyrightsVisible NOTIFY copyrightsVisibleChanged)
    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)

    // MapboxGL QML Type interface.
    Q_PROPERTY(QString style READ style WRITE setStyle NOTIFY styleChanged)
    Q_PROPERTY(qreal bearing READ bearing WRITE setBearing NOTIFY bearingChanged)

public:
    QQuickMapboxGL(QQuickItem *parent = 0);
    virtual ~QQuickMapboxGL();

    // QQuickFramebufferObject implementation.
    virtual Renderer *createRenderer() const Q_DECL_FINAL;

    // Map QML Type interface implementation.
    void setPlugin(QDeclarativeGeoServiceProvider *plugin);
    QDeclarativeGeoServiceProvider *plugin() const;

    void setMinimumZoomLevel(qreal minimumZoomLevel);
    qreal minimumZoomLevel() const;

    void setMaximumZoomLevel(qreal maximumZoomLevel);
    qreal maximumZoomLevel() const;

    void setZoomLevel(qreal zoomLevel);
    qreal zoomLevel() const;

    QGeoCoordinate center() const;

    QGeoServiceProvider::Error error() const;
    QString errorString() const;

    void setVisibleRegion(const QGeoShape &shape);
    QGeoShape visibleRegion() const;

    void setCopyrightsVisible(bool visible);
    bool copyrightsVisible() const;

    void setColor(const QColor &color);
    QColor color() const;

    Q_INVOKABLE void pan(int dx, int dy);

    // MapboxGL QML Type interface.
    void setStyle(const QString &style);
    QString style() const;

    void setBearing(qreal bearing);
    qreal bearing() const;

    QPointF swapPan();

    enum SyncState {
        NothingNeedsSync = 0x00,
        ZoomNeedsSync    = 0x01,
        CenterNeedsSync  = 0x02,
        StyleNeedsSync   = 0x04,
        PanNeedsSync     = 0x08,
        BearingNeedsSync = 0x16,
    };

    int swapSyncState();

signals:
    void minimumZoomLevelChanged();
    void maximumZoomLevelChanged();
    void zoomLevelChanged(qreal zoomLevel);
    void centerChanged(const QGeoCoordinate &coordinate);

    // Compatibility with Map QML Type, but no-op.
    void pluginChanged(QDeclarativeGeoServiceProvider *plugin);
    void errorChanged();
    void copyrightLinkActivated(const QString &link);
    void copyrightsVisibleChanged(bool visible);
    void colorChanged(const QColor &color);

    void styleChanged();
    void bearingChanged(qreal angle);

public slots:
    void setCenter(const QGeoCoordinate &center);

private:
    qreal m_minimumZoomLevel = 0;
    qreal m_maximumZoomLevel = 20;
    qreal m_zoomLevel = 20;

    QPointF m_pan = QPointF(0, 0);

    QGeoCoordinate m_center;
    QGeoShape m_visibleRegion;

    QString m_style;
    qreal m_bearing = 0;

    int m_syncState = NothingNeedsSync;
};

#endif // QQUICKMAPBOXGL_H
