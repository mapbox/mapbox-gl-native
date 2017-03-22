#ifndef QMAPBOXGL_H
#define QMAPBOXGL_H

#include <QImage>
#include <QMapbox>
#include <QMargins>
#include <QObject>
#include <QPointF>
#include <QSize>
#include <QString>
#include <QStringList>

class QMapboxGLPrivate;

// This header follows the Qt coding style: https://wiki.qt.io/Qt_Coding_Style

class Q_DECL_EXPORT QMapboxGLSettings
{
public:
    QMapboxGLSettings();

    enum GLContextMode {
        UniqueGLContext = 0,
        SharedGLContext
    };

    enum ConstrainMode {
        NoConstrain = 0,
        ConstrainHeightOnly,
        ConstrainWidthAndHeight
    };

    enum ViewportMode {
        DefaultViewport = 0,
        FlippedYViewport
    };

    GLContextMode contextMode() const;
    void setContextMode(GLContextMode);

    ConstrainMode constrainMode() const;
    void setConstrainMode(ConstrainMode);

    ViewportMode viewportMode() const;
    void setViewportMode(ViewportMode);

    unsigned cacheDatabaseMaximumSize() const;
    void setCacheDatabaseMaximumSize(unsigned);

    QString cacheDatabasePath() const;
    void setCacheDatabasePath(const QString &);

    QString assetPath() const;
    void setAssetPath(const QString &);

    QString accessToken() const;
    void setAccessToken(const QString &);

    QString apiBaseUrl() const;
    void setApiBaseUrl(const QString &);

private:
    GLContextMode m_contextMode;
    ConstrainMode m_constrainMode;
    ViewportMode m_viewportMode;

    unsigned m_cacheMaximumSize;
    QString m_cacheDatabasePath;
    QString m_assetPath;
    QString m_accessToken;
    QString m_apiBaseUrl;
};

struct Q_DECL_EXPORT QMapboxGLCameraOptions {
    QVariant center;  // Coordinate
    QVariant anchor;  // QPointF
    QVariant zoom;    // double
    QVariant angle;   // double
    QVariant pitch;   // double
};

class Q_DECL_EXPORT QMapboxGL : public QObject
{
    Q_OBJECT
    Q_PROPERTY(double latitude READ latitude WRITE setLatitude)
    Q_PROPERTY(double longitude READ longitude WRITE setLongitude)
    Q_PROPERTY(double zoom READ zoom WRITE setZoom)
    Q_PROPERTY(double bearing READ bearing WRITE setBearing)
    Q_PROPERTY(double pitch READ pitch WRITE setPitch)
    Q_PROPERTY(QString styleJson READ styleJson WRITE setStyleJson)
    Q_PROPERTY(QString styleUrl READ styleUrl WRITE setStyleUrl)
    Q_PROPERTY(double scale READ scale WRITE setScale)
    Q_PROPERTY(QMapbox::Coordinate coordinate READ coordinate WRITE setCoordinate)
    Q_PROPERTY(QMargins margins READ margins WRITE setMargins)

public:
    enum MapChange {
        MapChangeRegionWillChange = 0,
        MapChangeRegionWillChangeAnimated,
        MapChangeRegionIsChanging,
        MapChangeRegionDidChange,
        MapChangeRegionDidChangeAnimated,
        MapChangeWillStartLoadingMap,
        MapChangeDidFinishLoadingMap,
        MapChangeDidFailLoadingMap,
        MapChangeWillStartRenderingFrame,
        MapChangeDidFinishRenderingFrame,
        MapChangeDidFinishRenderingFrameFullyRendered,
        MapChangeWillStartRenderingMap,
        MapChangeDidFinishRenderingMap,
        MapChangeDidFinishRenderingMapFullyRendered,
        MapChangeDidFinishLoadingStyle,
        MapChangeSourceDidChange
    };

    // Determines the orientation of the map.
    enum NorthOrientation {
        NorthUpwards, // Default
        NorthRightwards,
        NorthDownwards,
        NorthLeftwards,
    };

    QMapboxGL(QObject* parent = 0,
              const QMapboxGLSettings& = QMapboxGLSettings(),
              const QSize& size = QSize(),
              qreal pixelRatio = 1);
    virtual ~QMapboxGL();

    void cycleDebugOptions();

    QString styleJson() const;
    QString styleUrl() const;

    void setStyleJson(const QString &);
    void setStyleUrl(const QString &);

    double latitude() const;
    void setLatitude(double latitude);

    double longitude() const;
    void setLongitude(double longitude);

    double scale() const;
    void setScale(double scale, const QPointF &center = QPointF());

    double zoom() const;
    void setZoom(double zoom);

    double minimumZoom() const;
    double maximumZoom() const;

    double bearing() const;
    void setBearing(double degrees);
    void setBearing(double degrees, const QPointF &center);

    double pitch() const;
    void setPitch(double pitch);

    NorthOrientation northOrientation() const;
    void setNorthOrientation(NorthOrientation);

    QMapbox::Coordinate coordinate() const;
    void setCoordinate(const QMapbox::Coordinate &);
    void setCoordinateZoom(const QMapbox::Coordinate &, double zoom);

    void jumpTo(const QMapboxGLCameraOptions&);

    void setGestureInProgress(bool inProgress);

    void addClass(const QString &);
    void removeClass(const QString &);
    bool hasClass(const QString &) const;
    void setClasses(const QStringList &);
    QStringList getClasses() const;

    void setTransitionOptions(qint64 duration, qint64 delay = 0);

    void addAnnotationIcon(const QString &name, const QImage &sprite);

    QMapbox::AnnotationID addAnnotation(const QMapbox::Annotation &);
    void updateAnnotation(QMapbox::AnnotationID, const QMapbox::Annotation &);
    void removeAnnotation(QMapbox::AnnotationID);

    void setLayoutProperty(const QString &layer, const QString &property, const QVariant &value);
    void setPaintProperty(const QString &layer, const QString &property, const QVariant &value, const QString &klass = QString());

    bool isFullyLoaded() const;

    void moveBy(const QPointF &offset);
    void scaleBy(double scale, const QPointF &center = QPointF());
    void rotateBy(const QPointF &first, const QPointF &second);

    void resize(const QSize &size, const QSize &framebufferSize);
    void setFramebufferObject(quint32 fbo);

    double metersPerPixelAtLatitude(double latitude, double zoom) const;
    QMapbox::ProjectedMeters projectedMetersForCoordinate(const QMapbox::Coordinate &) const;
    QMapbox::Coordinate coordinateForProjectedMeters(const QMapbox::ProjectedMeters &) const;
    QPointF pixelForCoordinate(const QMapbox::Coordinate &) const;
    QMapbox::Coordinate coordinateForPixel(const QPointF &) const;

    QMapbox::CoordinateZoom coordinateZoomForBounds(const QMapbox::Coordinate &sw, QMapbox::Coordinate &ne) const;
    QMapbox::CoordinateZoom coordinateZoomForBounds(const QMapbox::Coordinate &sw, QMapbox::Coordinate &ne, double bearing, double pitch);

    void setMargins(const QMargins &margins);
    QMargins margins() const;

    void addSource(const QString &sourceID, const QVariantMap& params);
    bool sourceExists(const QString &sourceID);
    void updateSource(const QString &sourceID, const QVariantMap& params);
    void removeSource(const QString &sourceID);

    void addImage(const QString &name, const QImage &sprite);
    void removeImage(const QString &name);

    void addCustomLayer(const QString &id,
        QMapbox::CustomLayerInitializeFunction,
        QMapbox::CustomLayerRenderFunction,
        QMapbox::CustomLayerDeinitializeFunction,
        void* context,
        char* before = NULL);
    void addLayer(const QVariantMap &params);
    bool layerExists(const QString &id);
    void removeLayer(const QString &id);

    void setFilter(const QString &layer, const QVariant &filter);

public slots:
    void render();
    void connectionEstablished();

signals:
    void needsRendering();
    void mapChanged(QMapboxGL::MapChange);
    void copyrightsChanged(const QString &copyrightsHtml);

private:
    Q_DISABLE_COPY(QMapboxGL)

    QMapboxGLPrivate *d_ptr;
};

Q_DECLARE_METATYPE(QMapboxGL::MapChange);

#endif // QMAPBOXGL_H
