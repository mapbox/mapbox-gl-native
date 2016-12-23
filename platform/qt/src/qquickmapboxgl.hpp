#pragma once

#include "qmapbox.hpp"
#include "qmapboxgl.hpp"
#include "qquickmapboxglmapparameter.hpp"

#include <QColor>
#include <QGeoCoordinate>
#include <QGeoServiceProvider>
#include <QGeoShape>
#include <QImage>
#include <QPointF>
#include <QQmlListProperty>
#include <QQuickFramebufferObject>

class QDeclarativeGeoServiceProvider;
class QQuickItem;
class QQuickMapboxGLRenderer;

class Q_DECL_EXPORT QQuickMapboxGL : public QQuickFramebufferObject
{
    Q_OBJECT

    // Map QML Type interface.
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

    // Proposed Qt interface - based on the example documentation below:
    // http://doc.qt.io/qt-5/qtqml-referenceexamples-properties-example.html
    Q_PROPERTY(QQmlListProperty<QQuickMapboxGLMapParameter> parameters READ parameters)

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

    // Proposed Qt interface implementation.
    QQmlListProperty<QQuickMapboxGLMapParameter> parameters();

protected:
    // QQmlParserStatus implementation
    void componentComplete() override;

signals:
    // Map QML Type signals.
    void minimumZoomLevelChanged();
    void maximumZoomLevelChanged();
    void zoomLevelChanged(qreal zoomLevel);
    void centerChanged(const QGeoCoordinate &coordinate);
    void colorChanged(const QColor &color);
    void errorChanged();

    // Compatibility with Map QML Type, but no-op.
    void pluginChanged(QDeclarativeGeoServiceProvider *plugin);
    void copyrightLinkActivated(const QString &link);
    void copyrightsVisibleChanged(bool visible);

public slots:
    void setCenter(const QGeoCoordinate &center);

private slots:
    void onMapChanged(QMapboxGL::MapChange);
    void onParameterPropertyUpdated(const QString &name);

private:
    static void appendParameter(QQmlListProperty<QQuickMapboxGLMapParameter> *prop, QQuickMapboxGLMapParameter *mapObject);
    static int countParameters(QQmlListProperty<QQuickMapboxGLMapParameter> *prop);
    static QQuickMapboxGLMapParameter *parameterAt(QQmlListProperty<QQuickMapboxGLMapParameter> *prop, int index);
    static void clearParameter(QQmlListProperty<QQuickMapboxGLMapParameter> *prop);

    enum SyncState {
        NothingNeedsSync = 0,
        ZoomNeedsSync    = 1 << 0,
        CenterNeedsSync  = 1 << 1,
        StyleNeedsSync   = 1 << 2,
        PanNeedsSync     = 1 << 3,
        BearingNeedsSync = 1 << 4,
        PitchNeedsSync   = 1 << 5,
    };

    struct Image {
        QString name;
        QImage sprite;
    };

    struct StyleProperty {
        enum Type {
            Paint = 0,
            Layout
        };

        Type type;
        QString layer;
        QString property;
        QVariant value;
        QString klass;
    };

    void processMapParameter(QQuickMapboxGLMapParameter *);
    bool parseImage(QQuickMapboxGLMapParameter *);
    bool parseStyle(QQuickMapboxGLMapParameter *);
    bool parseStyleProperty(QQuickMapboxGLMapParameter *, const QString &name);
    bool parseStyleLayer(QQuickMapboxGLMapParameter *);
    bool parseStyleSource(QQuickMapboxGLMapParameter *);
    bool parseStyleFilter(QQuickMapboxGLMapParameter *);
    bool parseBearing(QQuickMapboxGLMapParameter *);
    bool parsePitch(QQuickMapboxGLMapParameter *);

    qreal m_minimumZoomLevel = 0;
    qreal m_maximumZoomLevel = 20;
    qreal m_zoomLevel = 20;

    QPointF m_pan;

    QGeoCoordinate m_center;
    QGeoShape m_visibleRegion;
    QColor m_color;
    QString m_styleUrl;
    QList<Image> m_imageChanges;
    QList<StyleProperty> m_stylePropertyChanges;
    QList<QVariantMap> m_layerChanges;
    QList<QVariantMap> m_sourceChanges;
    QList<QVariantMap> m_filterChanges;
    QList<QQuickMapboxGLMapParameter*> m_parameters;

    QGeoServiceProvider::Error m_error = QGeoServiceProvider::NoError;
    QString m_errorString;

    qreal m_bearing = 0;
    qreal m_pitch = 0;

    int m_syncState = NothingNeedsSync;
    bool m_styleLoaded = false;

    friend class QQuickMapboxGLRenderer;
};
