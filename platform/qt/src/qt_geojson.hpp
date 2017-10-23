#pragma once

#include <mapbox/geojson.hpp>
#include <mbgl/util/geometry.hpp>
#include <mbgl/util/feature.hpp>

#include <QMapbox>

#include <QByteArray>
#include <QDebug>
#include <QVariant>

#include <string>

namespace QMapbox {

mbgl::Point<double> asMapboxGLPoint(const QMapbox::Coordinate &coordinate);
mbgl::MultiPoint<double> asMapboxGLMultiPoint(const QMapbox::Coordinates &multiPoint);
mbgl::LineString<double> asMapboxGLLineString(const QMapbox::Coordinates &lineString);
mbgl::MultiLineString<double> asMapboxGLMultiLineString(const QMapbox::CoordinatesCollection &multiLineString);
mbgl::Polygon<double> asMapboxGLPolygon(const QMapbox::CoordinatesCollection &polygon);
mbgl::MultiPolygon<double> asMapboxGLMultiPolygon(const QMapbox::CoordinatesCollections &multiPolygon);
mbgl::Value asMapboxGLPropertyValue(const QVariant &value);
mbgl::FeatureIdentifier asMapboxGLFeatureIdentifier(const QVariant &id);
mbgl::Feature asMapboxGLFeature(const QMapbox::Feature &feature);

} // namespace QMapbox
