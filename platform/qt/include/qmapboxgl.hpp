#ifndef QMAPBOXGL_H
#define QMAPBOXGL_H

#include <QObject>
#include <QPair>
#include <QPointF>

class QImage;
class QSize;
class QString;

class QMapboxGLPrivate;

// This header follows the Qt coding style: https://wiki.qt.io/Qt_Coding_Style

class Q_DECL_EXPORT QMapboxGLSettings
{
public:
    QMapboxGLSettings();

    unsigned cacheDatabaseMaximumSize() const;
    void setCacheDatabaseMaximumSize(unsigned);

    QString cacheDatabasePath() const;
    void setCacheDatabasePath(const QString &);

    QString assetPath() const;
    void setAssetPath(const QString &);

    QString accessToken() const;
    void setAccessToken(const QString &);

private:
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

public:
    typedef QPair<double, double> Coordinate;

    QMapboxGL(QObject *parent = 0, const QMapboxGLSettings& = QMapboxGLSettings());
    virtual ~QMapboxGL();

    void setStyleJSON(const QString &);
    void setStyleURL(const QString &);

    double latitude() const;
    void setLatitude(double latitude);

    double longitude() const;
    void setLongitude(double longitude);

    double scale() const;
    void setScale(double scale, const QPointF &center = QPointF(), int milliseconds = 0);

    double zoom() const;
    void setZoom(double zoom, int milliseconds = 0);

    double minimumZoom() const;
    double maximumZoom() const;

    double bearing() const;
    void setBearing(double degrees, int milliseconds = 0);
    void setBearing(double degrees, const QPointF &center);

    double pitch() const;
    void setPitch(double pitch, int milliseconds = 0);

    Coordinate coordinate() const;
    void setCoordinate(const Coordinate &, int milliseconds = 0);
    void setCoordinateZoom(const Coordinate &, double zoom, int milliseconds = 0);

    void setGestureInProgress(bool inProgress);

    bool isRotating() const;
    bool isScaling() const;
    bool isPanning() const;
    bool isFullyLoaded() const;

    void moveBy(const QPointF &offset);
    void scaleBy(double scale, const QPointF &center = QPointF(), int milliseconds = 0);
    void rotateBy(const QPointF &first, const QPointF &second);

    void resize(const QSize &size);

    void addAnnotationIcon(const QString &name, const QImage &sprite);

    QPointF pixelForCoordinate(const Coordinate &) const;
    Coordinate coordinateForPixel(const QPointF &) const;

public slots:
    void render();

signals:
    void needsRendering();
    void mapRegionDidChange();

private:
    Q_DISABLE_COPY(QMapboxGL)

    QMapboxGLPrivate *d_ptr;
};

#endif // QMAPBOXGL_H
