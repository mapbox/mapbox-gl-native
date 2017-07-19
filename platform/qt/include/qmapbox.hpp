#ifndef QMAPBOX_H
#define QMAPBOX_H

#include <QColor>
#include <QList>
#include <QPair>
#include <QVariant>
#include <QString>

// This header follows the Qt coding style: https://wiki.qt.io/Qt_Coding_Style

namespace QMapbox {

typedef QPair<double, double> Coordinate;
typedef QPair<Coordinate, double> CoordinateZoom;
typedef QPair<double, double> ProjectedMeters;

typedef QList<Coordinate> Coordinates;
typedef QList<Coordinates> CoordinatesCollection;

typedef QList<CoordinatesCollection> CoordinatesCollections;

struct Q_DECL_EXPORT Feature {
    enum Type {
        PointType = 1,
        LineStringType,
        PolygonType
    };

    Feature(Type type_ = PointType, const CoordinatesCollections& geometry_ = CoordinatesCollections(),
            const QVariantMap& properties_ = QVariantMap(), const QVariant& id_ = QVariant())
        : type(type_), geometry(geometry_), properties(properties_), id(id_) {}

    Type type;
    CoordinatesCollections geometry;
    QVariantMap properties;
    QVariant id;
};

struct Q_DECL_EXPORT ShapeAnnotationGeometry {
    enum Type {
        LineStringType = 1,
        PolygonType,
        MultiLineStringType,
        MultiPolygonType
    };

    ShapeAnnotationGeometry(Type type_ = LineStringType, const CoordinatesCollections& geometry_ = CoordinatesCollections())
        : type(type_), geometry(geometry_) {}

    Type type;
    CoordinatesCollections geometry;
};

struct Q_DECL_EXPORT SymbolAnnotation {
    Coordinate geometry;
    QString icon;
};

struct Q_DECL_EXPORT LineAnnotation {
    LineAnnotation(const ShapeAnnotationGeometry& geometry_ = ShapeAnnotationGeometry(), float opacity_ = 1.0f,
            float width_ = 1.0f, const QColor& color_ = Qt::black)
        : geometry(geometry_), opacity(opacity_), width(width_), color(color_) {}

    ShapeAnnotationGeometry geometry;
    float opacity;
    float width;
    QColor color;
};

struct Q_DECL_EXPORT FillAnnotation {
    FillAnnotation(const ShapeAnnotationGeometry& geometry_ = ShapeAnnotationGeometry(), float opacity_ = 1.0f,
            const QColor& color_ = Qt::black, const QVariant& outlineColor_ = QVariant())
        : geometry(geometry_), opacity(opacity_), color(color_), outlineColor(outlineColor_) {}

    ShapeAnnotationGeometry geometry;
    float opacity;
    QColor color;
    QVariant outlineColor;
};

typedef QVariant Annotation;
typedef quint32 AnnotationID;
typedef QList<AnnotationID> AnnotationIDs;

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

#endif // QMAPBOX_H
