#ifndef QMAPBOX_H
#define QMAPBOX_H

#include <QColor>
#include <QList>
#include <QPair>
#include <QVariant>
#include <QString>

// This header follows the Qt coding style: https://wiki.qt.io/Qt_Coding_Style

namespace QMapbox {

// Reflects mbgl::Point<double>.
typedef QPair<double, double> Coordinate;
typedef QPair<Coordinate, double> CoordinateZoom;

// Reflects mbgl::{LineString,LinearRing,MultiPoint}<double>.
typedef QList<Coordinate> Coordinates;

// Reflects mbgl::{MultiLineString,Polygon}<double>.
typedef QList<Coordinates> CoordinatesCollection;

// Reflects mbgl::MultiPolygon<double>.
typedef QList<CoordinatesCollection> CoordinatesCollections;

// Reflects mbgl::Feature.
struct Q_DECL_EXPORT Feature {
    enum Type {
        PointType = 1,
        LineStringType,
        PolygonType
    };
    Type type;
    CoordinatesCollections geometry;
    QVariantMap properties;
    QVariant id;
};

// Reflects mbgl::ShapeAnnotationGeometry.
struct Q_DECL_EXPORT ShapeAnnotationGeometry {
    enum Type {
        LineStringType,
        PolygonType,
        MultiLineStringType,
        MultiPolygonType
    };
    Type type;
    CoordinatesCollections geometry;
};

// Reflects mbgl::SymbolAnnotation.
struct Q_DECL_EXPORT SymbolAnnotation {
    Coordinate geometry;
    QString icon;
};

// Reflects mbgl::LineAnnotation.
struct Q_DECL_EXPORT LineAnnotation {
    ShapeAnnotationGeometry geometry;
    float opacity = 1.0f;
    float width = 1.0f;
    QColor color = Qt::black;
};

// Reflects mbgl::FillAnnotation.
struct Q_DECL_EXPORT FillAnnotation {
    ShapeAnnotationGeometry geometry;
    float opacity = 1.0f;
    QColor color = Qt::black;
    QVariant outlineColor;
};

// Reflects mbgl::StyleSourcedAnnotation.
struct Q_DECL_EXPORT StyleSourcedAnnotation {
    ShapeAnnotationGeometry geometry;
    QString layerID;
};

// SymbolAnnotation, LineAnnotation, FillAnnotation, StyleSourcedAnnotation.
typedef QVariant Annotation;

typedef quint32 AnnotationID;
typedef QList<AnnotationID> AnnotationIDs;

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

} // namespace QMapbox

Q_DECLARE_METATYPE(QMapbox::Coordinate);
Q_DECLARE_METATYPE(QMapbox::Coordinates);
Q_DECLARE_METATYPE(QMapbox::CoordinatesCollection);
Q_DECLARE_METATYPE(QMapbox::CoordinatesCollections);
Q_DECLARE_METATYPE(QMapbox::Feature);

Q_DECLARE_METATYPE(QMapbox::SymbolAnnotation);
Q_DECLARE_METATYPE(QMapbox::ShapeAnnotationGeometry);
Q_DECLARE_METATYPE(QMapbox::LineAnnotation);
Q_DECLARE_METATYPE(QMapbox::FillAnnotation);
Q_DECLARE_METATYPE(QMapbox::StyleSourcedAnnotation);

#endif // QMAPBOX_H
