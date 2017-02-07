#pragma once

#include <mapbox/geojson.hpp>
#include <mbgl/style/conversion/geojson.hpp>
#include <mbgl/util/rapidjson.hpp>

#include <QMapbox>

#include <QByteArray>
#include <QVariant>

#include <sstream>
#include <string>

namespace QMapbox {

mbgl::LineString<double> asMapboxGLLineString(const QMapbox::Coordinates &lineString) {
    mbgl::LineString<double> mbglLineString;
    mbglLineString.reserve(lineString.size());
    for (const auto &coordinate : lineString) {
        mbglLineString.emplace_back(mbgl::Point<double> { coordinate.second, coordinate.first });
    }
    return mbglLineString;
};

mbgl::MultiLineString<double> asMapboxGLMultiLineString(const QMapbox::CoordinatesCollection &multiLineString) {
    mbgl::MultiLineString<double> mbglMultiLineString;
    mbglMultiLineString.reserve(multiLineString.size());
    for (const auto &lineString : multiLineString) {
        mbglMultiLineString.emplace_back(std::forward<mbgl::LineString<double>>(asMapboxGLLineString(lineString)));
    }
    return mbglMultiLineString;
};

mbgl::Polygon<double> asMapboxGLPolygon(const QMapbox::CoordinatesCollection &polygon) {
    mbgl::Polygon<double> mbglPolygon;
    mbglPolygon.reserve(polygon.size());
    for (const auto &linearRing : polygon) {
        mbgl::LinearRing<double> mbglLinearRing;
        mbglLinearRing.reserve(linearRing.size());
        for (const auto &coordinate: linearRing) {
            mbglLinearRing.emplace_back(mbgl::Point<double> { coordinate.second, coordinate.first });
        }
        mbglPolygon.emplace_back(std::move(mbglLinearRing));
    }
    return mbglPolygon;
};

mbgl::MultiPolygon<double> asMapboxGLMultiPolygon(const QMapbox::CoordinatesCollections &multiPolygon) {
    mbgl::MultiPolygon<double> mbglMultiPolygon;
    mbglMultiPolygon.reserve(multiPolygon.size());
    for (const auto &polygon : multiPolygon) {
        mbglMultiPolygon.emplace_back(std::forward<mbgl::Polygon<double>>(asMapboxGLPolygon(polygon)));
    }
    return mbglMultiPolygon;
};

} // namespace QMapbox

namespace mbgl {
namespace style {
namespace conversion {

template <>
Result<GeoJSON> convertGeoJSON(const QVariant& value) {
    if (value.type() != QVariant::ByteArray) {
        return Error { "JSON data must be in QByteArray" };
    }

    auto data = value.toByteArray();

    rapidjson::GenericDocument<rapidjson::UTF8<>, rapidjson::CrtAllocator> d;
    if (data.endsWith(char(0))) {
        d.Parse<0>(value.toByteArray().data());
    } else {
        d.Parse<0>(value.toByteArray().constData());
    }

    if (d.HasParseError()) {
        std::stringstream message;
        message << d.GetErrorOffset() << " - " << rapidjson::GetParseError_En(d.GetParseError());

        return Error { message.str() };
    }

    conversion::Result<GeoJSON> geoJSON = conversion::convertGeoJSON<JSValue>(d);
    if (!geoJSON) {
        return Error { geoJSON.error().message };
    }

    return geoJSON;
}

} // namespace conversion
} // namespace style
} // namespace mbgl
