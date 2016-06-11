#ifndef QMAPBOX_H
#define QMAPBOX_H

#include <QList>
#include <QPair>
#include <QVariant>
#include <QString>

// This header follows the Qt coding style: https://wiki.qt.io/Qt_Coding_Style

namespace QMapbox {

typedef QPair<double, double> Coordinate;
typedef QList<Coordinate> Coordinates;
typedef QList<Coordinates> CoordinateSegments;

typedef QPair<Coordinate, double> CoordinateZoom;

typedef quint32 AnnotationID;
typedef QList<AnnotationID> AnnotationIDs;

typedef QPair<Coordinate, QString> PointAnnotation;

// FIXME: We need to add support for custom style properties
typedef QPair<CoordinateSegments, QString> ShapeAnnotation;

enum NetworkMode {
    Online, // Default
    Offline,
};

struct Q_DECL_EXPORT CameraOptions {
    QVariant center;  // Coordinate
    QVariant anchor;  // QPointF
    QVariant zoom;    // double
    QVariant angle;   // double
    QVariant pitch;   // double
};

Q_DECL_EXPORT QList<QPair<QString, QString> >& defaultStyles();

Q_DECL_EXPORT NetworkMode networkMode();
Q_DECL_EXPORT void setNetworkMode(NetworkMode);

// This struct is a 1:1 copy of mbgl::CustomLayerRenderParameters.
struct Q_DECL_EXPORT CustomLayerRenderParameters {
    double width;
    double height;
    double latitude;
    double longitude;
    double zoom;
    double bearing;
    double pitch;
    double altitude;
};

struct Q_DECL_EXPORT TransitionOptions {
    QVariant duration; // qint64
    QVariant delay; // qint64
};

typedef void (*CustomLayerInitializeFunction)(void* context) ;
typedef void (*CustomLayerRenderFunction)(void* context, const CustomLayerRenderParameters&);
typedef void (*CustomLayerDeinitializeFunction)(void* context);

Q_DECL_EXPORT void initializeGLExtensions();

}

Q_DECLARE_METATYPE(QMapbox::Coordinate);

#endif // QMAPBOX_H
