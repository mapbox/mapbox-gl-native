#include "qmapbox.hpp"

#include <mbgl/storage/network_status.hpp>
#include <mbgl/util/default_styles.hpp>
#include <mbgl/util/geometry.hpp>
#include <mbgl/util/traits.hpp>
#include <mbgl/util/projection.hpp>

#include <QOpenGLContext>

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

    Alias for QVector<QMapbox::Coordinate>.
    A list of QMapbox::Coordinate objects.
*/

/*!
    \typedef QMapbox::CoordinatesCollection

    Alias for QVector<QMapbox::Coordinates>.
    A list of QMapbox::Coordinates objects.
*/

/*!
    \typedef QMapbox::CoordinatesCollections

    Alias for QVector<QMapbox::CoordinatesCollection>.
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

    Alias for QVector<quint32> representing a container of annotation identifiers.
*/

/*!
    \class QMapbox::CustomLayerHostInterface

    Represents a host interface to be implemented for rendering custom layers.

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
    \fn QVector<QPair<QString, QString> >& QMapbox::defaultStyles()

    Returns a list containing a pair of string objects, representing the style
    URL and name, respectively.
*/
QVector<QPair<QString, QString> >& defaultStyles()
{
    static QVector<QPair<QString, QString>> styles;

    if (styles.isEmpty()) {
        for (auto style : mbgl::util::default_styles::orderedStyles) {
            styles.append(QPair<QString, QString>(
                QString::fromStdString(style.url), QString::fromStdString(style.name)));
        }
    }

    return styles;
}

/*!
    Returns the amount of meters per pixel from a given \a latitude and \a zoom.
*/
double metersPerPixelAtLatitude(double latitude, double zoom)
{
    return mbgl::Projection::getMetersPerPixelAtLatitude(latitude, zoom);
}

/*!
    Return the projected meters for a given \a coordinate object.
*/
ProjectedMeters projectedMetersForCoordinate(const Coordinate &coordinate)
{
    auto projectedMeters = mbgl::Projection::projectedMetersForLatLng(mbgl::LatLng { coordinate.first, coordinate.second });
    return QMapbox::ProjectedMeters(projectedMeters.northing(), projectedMeters.easting());
}

/*!
    Returns the coordinate for a given \a projectedMeters object.
*/
Coordinate coordinateForProjectedMeters(const ProjectedMeters &projectedMeters)
{
    auto latLng = mbgl::Projection::latLngForProjectedMeters(mbgl::ProjectedMeters { projectedMeters.first, projectedMeters.second });
    return QMapbox::Coordinate(latLng.latitude(), latLng.longitude());
}

} // namespace QMapbox
