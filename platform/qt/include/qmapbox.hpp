#ifndef QMAPBOX_H
#define QMAPBOX_H

#include <QList>
#include <QPair>
#include <QVariant>
#include <QString>

// This header follows the Qt coding style: https://wiki.qt.io/Qt_Coding_Style

namespace QMapbox {

typedef QPair<double, double> Coordinate;
typedef QList<Coordinate> LineString;

typedef QPair<Coordinate, double> CoordinateZoom;

typedef quint32 AnnotationID;
typedef QList<AnnotationID> AnnotationIDs;

typedef QPair<Coordinate, QString> PointAnnotation;

// FIXME: We need to add support for custom style properties
typedef QPair<LineString, QString> ShapeAnnotation;

// Reflects mbgl::NetworkStatus::Status.
enum NetworkMode {
    Online, // Default
    Offline,
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

typedef void (*CustomLayerInitializeFunction)(void* context) ;
typedef void (*CustomLayerRenderFunction)(void* context, const CustomLayerRenderParameters&);
typedef void (*CustomLayerDeinitializeFunction)(void* context);

Q_DECL_EXPORT void initializeGLExtensions();
Q_DECL_EXPORT void registerTypes();

} // namespace QMapbox

Q_DECLARE_METATYPE(QMapbox::Coordinate);

#endif // QMAPBOX_H
