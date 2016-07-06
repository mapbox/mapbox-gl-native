#ifndef QMAPBOXGL_H
#define QMAPBOXGL_H

#include <QMapbox>
#include <QObject>
#include <QPointF>

class QImage;
class QMargins;
class QSize;
class QString;
class QStringList;

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
    Q_ENUMS(MapChange)

public:
    // Determines the orientation of the map.
    enum NorthOrientation {
        NorthUpwards, // Default
        NorthRightwards,
        NorthDownwards,
        NorthLeftwards,
    };

    // Reflects mbgl::MapChange.
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
        MapChangeDidFinishRenderingMapFullyRendered
    };

    QMapboxGL(QObject *parent = 0, const QMapboxGLSettings& = QMapboxGLSettings());
    virtual ~QMapboxGL();

    void cycleDebugOptions();

    QString styleJSON() const;
    QString styleURL() const;

    void setStyleJSON(const QString &);
    void setStyleURL(const QString &);

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
    void addClass(const QString &, const QMapbox::TransitionOptions &);
    void removeClass(const QString &);
    void removeClass(const QString &, const QMapbox::TransitionOptions &);
    bool hasClass(const QString &) const;
    void setClasses(const QStringList &);
    void setClasses(const QStringList &, const QMapbox::TransitionOptions &);
    QStringList getClasses() const;

    QMapbox::AnnotationID addPointAnnotation(const QMapbox::PointAnnotation &);
    QMapbox::AnnotationID addShapeAnnotation(const QMapbox::ShapeAnnotation &);

    void updatePointAnnotation(QMapbox::AnnotationID, const QMapbox::PointAnnotation &);

    void removeAnnotation(QMapbox::AnnotationID);

    bool isRotating() const;
    bool isScaling() const;
    bool isPanning() const;
    bool isFullyLoaded() const;

    void moveBy(const QPointF &offset);
    void scaleBy(double scale, const QPointF &center = QPointF());
    void rotateBy(const QPointF &first, const QPointF &second);

    void resize(const QSize &size);

    void addAnnotationIcon(const QString &name, const QImage &sprite);

    QPointF pixelForCoordinate(const QMapbox::Coordinate &) const;
    QMapbox::Coordinate coordinateForPixel(const QPointF &) const;

    QMapbox::CoordinateZoom coordinateZoomForBounds(const QMapbox::Coordinate &sw, QMapbox::Coordinate &ne) const;
    QMapbox::CoordinateZoom coordinateZoomForBounds(const QMapbox::Coordinate &sw, QMapbox::Coordinate &ne, double bearing, double pitch);

    void setMargins(const QMargins &margins);
    QMargins margins() const;

    void addCustomLayer(const QString &id,
        QMapbox::CustomLayerInitializeFunction,
        QMapbox::CustomLayerRenderFunction,
        QMapbox::CustomLayerDeinitializeFunction,
        void* context,
        char* before = NULL);

    void setFilter(const QString &layerID, const QMapbox::Filter &filter);
    void addLayer(const QMapbox::Layer &layer);
    void removeLayer(const QString &layerID);
    void addSource(const QMapbox::Source &source);
    void removeSource(const QString &sourceID);
    void setPaintProperty(const QString &layerID, QMapbox::PaintPropertyType, const QMapbox::PropertyValue &, const QString &klass = QString());
    void setLayoutProperty(const QString &layerID, QMapbox::LayoutPropertyType, const QMapbox::PropertyValue &);

public slots:
    void render();
    void connectionEstablished();

signals:
    void needsRendering();
    void mapChanged(QMapboxGL::MapChange);

private:
    Q_DISABLE_COPY(QMapboxGL)

    QMapboxGLPrivate *d_ptr;
};

Q_DECLARE_METATYPE(QMapboxGL::MapChange);

#endif // QMAPBOXGL_H
