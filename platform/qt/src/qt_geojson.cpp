#include "qt_geojson.hpp"
#include <mapbox/geojson.hpp>
#include <mbgl/util/geometry.hpp>
#include <mbgl/util/feature.hpp>

namespace QMapbox {

mbgl::Point<double> asMapboxGLPoint(const QMapbox::Coordinate &coordinate) {
    return mbgl::Point<double> { coordinate.second, coordinate.first };
}

mbgl::MultiPoint<double> asMapboxGLMultiPoint(const QMapbox::Coordinates &multiPoint) {
    mbgl::MultiPoint<double> mbglMultiPoint;
    mbglMultiPoint.reserve(multiPoint.size());
    for (const auto &point: multiPoint) {
        mbglMultiPoint.emplace_back(asMapboxGLPoint(point));
    }
    return mbglMultiPoint;
};

mbgl::LineString<double> asMapboxGLLineString(const QMapbox::Coordinates &lineString) {
    mbgl::LineString<double> mbglLineString;
    mbglLineString.reserve(lineString.size());
    for (const auto &coordinate : lineString) {
        mbglLineString.emplace_back(asMapboxGLPoint(coordinate));
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
            mbglLinearRing.emplace_back(asMapboxGLPoint(coordinate));
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

mbgl::Value asMapboxGLPropertyValue(const QVariant &value) {
    auto valueList = [](const QVariantList &list) {
        std::vector<mbgl::Value> mbglList;
        mbglList.reserve(list.size());
        for (const auto& listValue : list) {
            mbglList.emplace_back(asMapboxGLPropertyValue(listValue));
        }
        return mbglList;
    };

    auto valueMap = [](const QVariantMap &map) {
        std::unordered_map<std::string, mbgl::Value> mbglMap;
        mbglMap.reserve(map.size());
        for (auto it = map.constBegin(); it != map.constEnd(); ++it) {
            mbglMap.emplace(std::make_pair(it.key().toStdString(), asMapboxGLPropertyValue(it.value())));
        }
        return mbglMap;
    };

    switch (value.type()) {
#if QT_VERSION >= 0x050000
    case QMetaType::UnknownType:
#else
    case QVariant::Invalid:
#endif
        return mbgl::NullValue {};
    case QMetaType::Bool:
        return { value.toBool() };
    case QMetaType::ULongLong:
        return { uint64_t(value.toULongLong()) };
    case QMetaType::LongLong:
        return { int64_t(value.toLongLong()) };
    case QMetaType::Double:
        return { value.toDouble() };
    case QMetaType::QString:
        return { value.toString().toStdString() };
    case QMetaType::QVariantList:
        return valueList(value.toList());
    case QMetaType::QVariantMap:
        return valueMap(value.toMap());
    default:
        qWarning() << "Unsupported feature property value:" << value;
        return {};
    }
}

mbgl::FeatureIdentifier asMapboxGLFeatureIdentifier(const QVariant &id) {
    switch (id.type()) {
#if QT_VERSION >= 0x050000
    case QMetaType::UnknownType:
#else
    case QVariant::Invalid:
#endif
        return {};
    case QMetaType::ULongLong:
        return { uint64_t(id.toULongLong()) };
    case QMetaType::LongLong:
        return { int64_t(id.toLongLong()) };
    case QMetaType::Double:
        return { id.toDouble() };
    case QMetaType::QString:
        return { id.toString().toStdString() };
    default:
        qWarning() << "Unsupported feature identifier:" << id;
        return {};
    }
}

mbgl::Feature asMapboxGLFeature(const QMapbox::Feature &feature) {
    mbgl::PropertyMap properties;
    properties.reserve(feature.properties.size());
    for (auto it = feature.properties.constBegin(); it != feature.properties.constEnd(); ++it) {
        properties.emplace(std::make_pair(it.key().toStdString(), asMapboxGLPropertyValue(it.value())));
    }

    mbgl::FeatureIdentifier id = asMapboxGLFeatureIdentifier(feature.id);

    if (feature.type == QMapbox::Feature::PointType) {
        const QMapbox::Coordinates &points = feature.geometry.first().first();
        if (points.size() == 1) {
            return { asMapboxGLPoint(points.first()), std::move(properties), std::move(id) };
        } else {
            return { asMapboxGLMultiPoint(points), std::move(properties), std::move(id) };
        }
    } else if (feature.type == QMapbox::Feature::LineStringType) {
        const QMapbox::CoordinatesCollection &lineStrings = feature.geometry.first();
        if (lineStrings.size() == 1) {
            return { asMapboxGLLineString(lineStrings.first()), std::move(properties), std::move(id) };
        } else {
            return { asMapboxGLMultiLineString(lineStrings), std::move(properties), std::move(id) };
        }
    } else { // PolygonType
        const QMapbox::CoordinatesCollections &polygons = feature.geometry;
        if (polygons.size() == 1) {
            return { asMapboxGLPolygon(polygons.first()), std::move(properties), std::move(id) };
        } else {
            return { asMapboxGLMultiPolygon(polygons), std::move(properties), std::move(id) };
        }
    }
};

} // namespace QMapbox
