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

// Runtime Style API

enum FilterValueType {
    NullFilterValueType,
    BooleanFilterValueType,
    UnsignedIntegerFilterValueType,
    SignedIntegerFilterValueType,
    DoubleFilterValueType,
    StringFilterValueType,
    ListFilterValueType,
    MapFilterValueType,
};

enum FilterType {
    EqualsFilterType,
    NotEqualsFilterType,
    GreaterThanFilterType,
    GreaterThanEqualsFilterType,
    LessThanFilterType,
    LessThanEqualsFilterType,
    InFilterType,
    NotInFilterType,
    AllFilterType,
    AnyFilterType,
    NoneFilterType,
    HasFilterType,
    NotHasFilterType,
};

struct Q_DECL_EXPORT FilterValue {
    QVariant value;
    FilterValueType type;
};

typedef QList<FilterValue> FilterValueList;
typedef QMap<QString, FilterValue> FilterValueMap;

// BinaryFilterForm, SetFilterForm, CompoundFilterForm, UnaryFilterForm
typedef QVariant FilterForm;

struct Q_DECL_EXPORT Filter {
    FilterType type;
    FilterForm form;
};

struct Q_DECL_EXPORT BinaryFilterForm {
    QString key;
    FilterValue value;
};

struct Q_DECL_EXPORT SetFilterForm {
    QString key;
    FilterValueList values;
};

struct Q_DECL_EXPORT CompoundFilterForm {
    QList<Filter> filters;
};

struct Q_DECL_EXPORT UnaryFilterForm {
    QString key;
};

enum LayerType {
    FillLayer,
    LineLayer,
    CircleLayer,
    SymbolLayer,
    RasterLayer,
    BackgroundLayer,
};

struct Q_DECL_EXPORT Layer {
    LayerType type;
    QString layerID;
    QVariant sourceID; // QString
    QVariant sourceLayer; // QString
    QVariant filter; // Filter
    QVariant minZoom; // float
    QVariant maxZoom; // float
};

} // namespace QMapbox

Q_DECLARE_METATYPE(QMapbox::Coordinate);

Q_DECLARE_METATYPE(QMapbox::Filter);
Q_DECLARE_METATYPE(QMapbox::FilterValueList);
Q_DECLARE_METATYPE(QMapbox::FilterValueMap);
Q_DECLARE_METATYPE(QMapbox::BinaryFilterForm);
Q_DECLARE_METATYPE(QMapbox::SetFilterForm);
Q_DECLARE_METATYPE(QMapbox::CompoundFilterForm);
Q_DECLARE_METATYPE(QMapbox::UnaryFilterForm);

#endif // QMAPBOX_H
