#include "qmapbox.hpp"

#include <mbgl/storage/network_status.hpp>
#include <mbgl/util/default_styles.hpp>
#include <mbgl/util/geometry.hpp>
#include <mbgl/util/traits.hpp>

#if QT_VERSION >= 0x050000
#include <QOpenGLContext>
#else
#include <QGLContext>
#endif

// mbgl::NetworkStatus::Status
static_assert(mbgl::underlying_type(QMapbox::Online) == mbgl::underlying_type(mbgl::NetworkStatus::Status::Online), "error");
static_assert(mbgl::underlying_type(QMapbox::Offline) == mbgl::underlying_type(mbgl::NetworkStatus::Status::Offline), "error");

// mbgl::FeatureType
static_assert(mbgl::underlying_type(QMapbox::Feature::PointType) == mbgl::underlying_type(mbgl::FeatureType::Point), "error");
static_assert(mbgl::underlying_type(QMapbox::Feature::LineStringType) == mbgl::underlying_type(mbgl::FeatureType::LineString), "error");
static_assert(mbgl::underlying_type(QMapbox::Feature::PolygonType) == mbgl::underlying_type(mbgl::FeatureType::Polygon), "error");

namespace QMapbox {

/*!
    \namespace QMapbox
    \inmodule Mapbox Maps SDK for Qt

    Contains miscellaneous Mapbox bindings used throughout QMapboxGL.
*/

/*!
    \typedef QMapbox::Coordinate

    Alias for QPair<double, double>.
    Representation for geographical coordinates - latitude and longitude, respectively.
*/

/*!
    \typedef QMapbox::CoordinateZoom

    Alias for QPair<Coordinate, double>.
    Used as return value in QMapboxGL::coordinateZoomForBounds.
*/

/*!
    \typedef QMapbox::ProjectedMeters

    Alias for QPair<double, double>.
    Representation for projected meters - northing and easting, respectively.
*/

/*!
    \typedef QMapbox::Coordinates

    Alias for QList<QMapbox::Coordinate>.
    A list of QMapbox::Coordinate objects.
*/

/*!
    \typedef QMapbox::CoordinatesCollection

    Alias for QList<QMapbox::Coordinates>.
    A list of QMapbox::Coordinates objects.
*/

/*!
    \typedef QMapbox::CoordinatesCollections

    Alias for QList<QMapbox::CoordinatesCollection>.
    A list of QMapbox::CoordinatesCollection objects.
*/

/*!
    \class QMapbox::Feature

    \inmodule Mapbox Maps SDK for Qt

    Represents \l {https://www.mapbox.com/help/define-features/}{map features}
    via its \a type (PointType, LineStringType or PolygonType), \a geometry, \a
    properties map and \a id (optional).
*/

/*!
    \enum QMapbox::Feature::Type

    This enum is used as basis for geometry disambiguation in QMapbox::Feature.

    \value PointType      A point geometry type. Means a single or a collection of points.
    \value LineStringType A line string geometry type. Means a single or a collection of line strings.
    \value PolygonType    A polygon geometry type. Means a single or a collection of polygons.
*/

/*!
    \class QMapbox::ShapeAnnotationGeometry

    \inmodule Mapbox Maps SDK for Qt

    Represents a shape annotation geometry.
*/

/*!
    \enum QMapbox::ShapeAnnotationGeometry::Type

    This enum is used as basis for shape annotation geometry disambiguation.

    \value PolygonType         A polygon geometry type.
    \value LineStringType      A line string geometry type.
    \value MultiPolygonType    A polygon geometry collection type.
    \value MultiLineStringType A line string geometry collection type.
*/

/*!
    \class QMapbox::SymbolAnnotation

    \inmodule Mapbox Maps SDK for Qt

    A symbol annotation comprises of its geometry and an icon identifier.
*/

/*!
    \class QMapbox::LineAnnotation

    \inmodule Mapbox Maps SDK for Qt

    Represents a line annotation object, along with its properties.

    A line annotation comprises of its geometry and line properties such as opacity, width and color.
*/

/*!
    \class QMapbox::FillAnnotation

    \inmodule Mapbox Maps SDK for Qt

    Represents a fill annotation object, along with its properties.

    A fill annotation comprises of its geometry and fill properties such as opacity, color and outline color.
*/

/*!
    \typedef QMapbox::Annotation

    Alias for QVariant.
    Container that encapsulates either a symbol, a line, a fill or a style sourced annotation.
*/

/*!
    \typedef QMapbox::AnnotationID

    Alias for quint32 representing an annotation identifier.
*/

/*!
    \typedef QMapbox::AnnotationIDs

    Alias for QList<quint32> representing a container of annotation identifiers.
*/

/*!
    \typedef QMapbox::CustomLayerDeinitializeFunction

    Represents a callback to be called when destroying a custom layer.

    \warning This is used for delegating the rendering of a layer to the user of
    this API and is not officially supported. Use at your own risk.
*/

/*!
    \typedef QMapbox::CustomLayerInitializeFunction

    Represents a callback to be called when initializing a custom layer.

    \warning This is used for delegating the rendering of a layer to the user of
    this API and is not officially supported. Use at your own risk.
*/

/*!
    \typedef QMapbox::CustomLayerRenderFunction

    Represents a callback to be called on each render pass for a custom layer.

    \warning This is used for delegating the rendering of a layer to the user of
    this API and is not officially supported. Use at your own risk.
*/

/*!
    \enum QMapbox::NetworkMode

    This enum represents whether server requests can be performed via network.

    \value Online  Server network requests are accessible.
    \value Offline Only requests to the local cache are accessible.
*/

/*!
    \class QMapbox::CustomLayerRenderParameters
    \inmodule Mapbox Maps SDK for Qt

    QMapbox::CustomLayerRenderParameters provides the data passed on each render
    pass for a custom layer.
*/

/*!
    \fn QMapbox::NetworkMode QMapbox::networkMode()

    Returns the current QMapbox::NetworkMode.
*/
NetworkMode networkMode()
{
    return static_cast<NetworkMode>(mbgl::NetworkStatus::Get());
}

/*!
    \fn void QMapbox::setNetworkMode(QMapbox::NetworkMode mode)

    Forwards the network status \a mode to Mapbox GL Native engine.

    File source requests uses the available network when \a mode is set to \b
    Online, otherwise scoped to the local cache.
*/
void setNetworkMode(NetworkMode mode)
{
    mbgl::NetworkStatus::Set(static_cast<mbgl::NetworkStatus::Status>(mode));
}

/*!
    \fn QList<QPair<QString, QString> >& QMapbox::defaultStyles()

    Returns a list containing a pair of string objects, representing the style
    URL and name, respectively.
*/
QList<QPair<QString, QString> >& defaultStyles()
{
    static QList<QPair<QString, QString>> styles;

    if (styles.isEmpty()) {
        for (auto style : mbgl::util::default_styles::orderedStyles) {
            styles.append(QPair<QString, QString>(
                QString::fromStdString(style.url), QString::fromStdString(style.name)));
        }
    }

    return styles;
}

} // namespace QMapbox
