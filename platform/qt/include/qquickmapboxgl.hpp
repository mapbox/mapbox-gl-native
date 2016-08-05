#ifndef QQUICKMAPBOXGL_H
#define QQUICKMAPBOXGL_H

#include <QColor>
#include <QGeoCoordinate>
#include <QGeoServiceProvider>
#include <QGeoShape>
#include <QPointF>
#include <QQuickFramebufferObject>

#include <QQuickMapboxGLStyle>

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
    Q_PROPERTY(QQuickMapboxGLStyle *style READ style WRITE setStyle NOTIFY styleChanged)
    Q_PROPERTY(qreal bearing READ bearing WRITE setBearing NOTIFY bearingChanged)
    Q_PROPERTY(qreal pitch READ pitch WRITE setPitch NOTIFY pitchChanged)

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

    QList<QVariantMap>& layoutPropertyChanges() { return m_layoutChanges; }
    QList<QVariantMap>& paintPropertyChanges() { return m_paintChanges; }

    // MapboxGL QML Type interface.
    void setStyle(QQuickMapboxGLStyle *);
    QQuickMapboxGLStyle* style() const;

    void setBearing(qreal bearing);
    qreal bearing() const;

    void setPitch(qreal pitch);
    qreal pitch() const;

    QPointF swapPan();

    enum SyncState {
        NothingNeedsSync = 0,
        ZoomNeedsSync    = 1 << 0,
        CenterNeedsSync  = 1 << 1,
        StyleNeedsSync   = 1 << 2,
        PanNeedsSync     = 1 << 3,
        BearingNeedsSync = 1 << 4,
        PitchNeedsSync   = 1 << 5,
    };

    int swapSyncState();

protected:
    // QQuickItem implementation.
    virtual void itemChange(QQuickItem::ItemChange change, const QQuickItem::ItemChangeData &value);

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
    void pitchChanged(qreal angle);

public slots:
    void setCenter(const QGeoCoordinate &center);

private slots:
    void onStyleChanged();
    void onStylePropertyUpdated(const QVariantMap &params);

private:
    qreal m_minimumZoomLevel = 0;
    qreal m_maximumZoomLevel = 20;
    qreal m_zoomLevel = 20;

    QPointF m_pan = QPointF(0, 0);

    QGeoCoordinate m_center;
    QGeoShape m_visibleRegion;
    QColor m_color;
    QList<QVariantMap> m_layoutChanges;
    QList<QVariantMap> m_paintChanges;

    QQuickMapboxGLStyle *m_style = 0;
    qreal m_bearing = 0;
    qreal m_pitch = 0;

    int m_syncState = NothingNeedsSync;
};

#endif // QQUICKMAPBOXGL_H
