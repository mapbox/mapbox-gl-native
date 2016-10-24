#ifndef QMAPBOXGL_H
#define QMAPBOXGL_H

#include <QMapbox>
#include <QObject>
#include <QSize>
#include <QPointF>

class QImage;
class QMargins;
class QSize;
class QString;
class QStringList;
class QOpenGLFramebufferObject;

class QMapboxGLPrivate;

// This header follows the Qt coding style: https://wiki.qt.io/Qt_Coding_Style

class Q_DECL_EXPORT QMapboxGLSettings
{
public:
    QMapboxGLSettings();

    enum MapMode {
        ContinuousMap = 0,
        StillMap
    };

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

    MapMode mapMode() const;
    void setMapMode(MapMode);

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

private:
    MapMode m_mapMode;
    GLContextMode m_contextMode;
    ConstrainMode m_constrainMode;
    ViewportMode m_viewportMode;

    unsigned m_cacheMaximumSize;
    QString m_cacheDatabasePath;
    QString m_assetPath;
    QString m_accessToken;
};

class Q_DECL_EXPORT QMapboxGL : public QObject
{
    Q_OBJECT
    Q_PROPERTY(double latitude READ latitude WRITE setLatitude)
    Q_PROPERTY(double longitude READ longitude WRITE setLongitude)
    Q_PROPERTY(double zoom READ zoom WRITE setZoom)
    Q_PROPERTY(double bearing READ bearing WRITE setBearing)
    Q_PROPERTY(double pitch READ pitch WRITE setPitch)

public:
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

    void jumpTo(const QMapbox::CameraOptions&);

    void setGestureInProgress(bool inProgress);

    void addClass(const QString &);
    void removeClass(const QString &);
    bool hasClass(const QString &) const;
    void setClasses(const QStringList &);
    QStringList getClasses() const;

    QMapbox::TransitionOptions getTransitionOptions() const;
    void setTransitionOptions(const QMapbox::TransitionOptions&);

    QMapbox::AnnotationID addPointAnnotation(const QMapbox::PointAnnotation &);
    QMapbox::AnnotationID addShapeAnnotation(const QMapbox::ShapeAnnotation &);

    void updatePointAnnotation(QMapbox::AnnotationID, const QMapbox::PointAnnotation &);

    void removeAnnotation(QMapbox::AnnotationID);

    void setLayoutProperty(const QString &layer, const QString &property, const QVariant &value);
    void setPaintProperty(const QString &layer, const QString &property, const QVariant &value, const QString &klass = QString());

    bool isRotating() const;
    bool isScaling() const;
    bool isPanning() const;
    bool isFullyLoaded() const;

    void moveBy(const QPointF &offset);
    void scaleBy(double scale, const QPointF &center = QPointF());
    void rotateBy(const QPointF &first, const QPointF &second);

    void resize(const QSize &size, const QSize &framebufferSize);

    void addAnnotationIcon(const QString &name, const QImage &sprite);

    QPointF pixelForCoordinate(const QMapbox::Coordinate &) const;
    QMapbox::Coordinate coordinateForPixel(const QPointF &) const;

    QMapbox::CoordinateZoom coordinateZoomForBounds(const QMapbox::Coordinate &sw, QMapbox::Coordinate &ne) const;
    QMapbox::CoordinateZoom coordinateZoomForBounds(const QMapbox::Coordinate &sw, QMapbox::Coordinate &ne, double bearing, double pitch);

    void setMargins(const QMargins &margins);
    QMargins margins() const;

    void addSource(const QString &sourceID, const QVariantMap& params);
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
    void removeLayer(const QString &id);

    void setFilter(const QString &layer, const QVariant &filter);

public slots:
#if QT_VERSION >= 0x050000
    void render(QOpenGLFramebufferObject *fbo = NULL);
#else
    void render();
#endif
    void connectionEstablished();

signals:
    void needsRendering();
    void mapChanged(QMapbox::MapChange);

private:
    Q_DISABLE_COPY(QMapboxGL)

    QMapboxGLPrivate *d_ptr;
};

#endif // QMAPBOXGL_H
