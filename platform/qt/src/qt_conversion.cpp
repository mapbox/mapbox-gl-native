#include <mbgl/util/feature.hpp>
#include <mbgl/style/conversion/geojson.hpp>
#include <QColor>
#include <QMapbox>
#include "qt_geojson.hpp"

namespace mbgl {
namespace style {
namespace conversion {

template<> bool ValueTraits<QVariant>::isUndefined(const QVariant& value) {
    return value.isNull() || !value.isValid();
}

template<> bool ValueTraits<QVariant>::isArray(const QVariant& value) {
    return value.canConvert(QVariant::List);
}

template<> std::size_t ValueTraits<QVariant>::arrayLength(const QVariant& value) {
    return value.toList().size();
}

template<> QVariant ValueTraits<QVariant>::arrayMember(const QVariant& value, std::size_t i) {
    return value.toList()[i];
}

template<> bool ValueTraits<QVariant>::isObject(const QVariant& value) {
    return value.canConvert(QVariant::Map)
        || value.type() == QVariant::ByteArray
#if QT_VERSION >= 0x050000
        || QString(value.typeName()) == QStringLiteral("QMapbox::Feature");
#else
        || QString(value.typeName()) == QString("QMapbox::Feature");
#endif
}

template<> optional<QVariant> ValueTraits<QVariant>::objectMember(const QVariant& value, const char* key) {
    auto map = value.toMap();
    auto iter = map.constFind(key);

    if (iter != map.constEnd()) {
        return iter.value();
    } else {
        return {};
    }
}

using EachMemberFn = std::function<optional<Error>(const std::string&, const QVariant&)>;

template<> optional<Error> ValueTraits<QVariant>::eachMember(const QVariant& value, const EachMemberFn& fn) {
    auto map = value.toMap();
    auto iter = map.constBegin();

    while (iter != map.constEnd()) {
        optional<Error> result = fn(iter.key().toStdString(), iter.value());
        if (result) {
            return result;
        }

        ++iter;
    }

    return {};
}

template<> optional<bool> ValueTraits<QVariant>::toBool(const QVariant& value) {
    if (value.type() == QVariant::Bool) {
        return value.toBool();
    } else {
        return {};
    }
}

template<> optional<float> ValueTraits<QVariant>::toNumber(const QVariant& value) {
    if (value.type() == QVariant::Int || value.type() == QVariant::Double) {
        return value.toFloat();
    } else {
        return {};
    }
}
template<> optional<double> ValueTraits<QVariant>::toDouble(const QVariant& value) {
    if (value.type() == QVariant::Int || value.type() == QVariant::Double) {
        return value.toDouble();
    } else {
        return {};
    }
}

template<> optional<std::string> ValueTraits<QVariant>::toString(const QVariant& value) {
    if (value.type() == QVariant::String) {
        return value.toString().toStdString();
    } else if (value.type() == QVariant::Color) {
        return value.value<QColor>().name().toStdString();
    } else {
        return {};
    }
}

template<> optional<mbgl::Value> ValueTraits<QVariant>::toValue(const QVariant& value) {
    if (value.type() == QVariant::Bool) {
        return { value.toBool() };
    } else if (value.type() == QVariant::String) {
        return { value.toString().toStdString() };
    } else if (value.type() == QVariant::Color) {
        return { value.value<QColor>().name().toStdString() };
    } else if (value.type() == QVariant::Int) {
        return { int64_t(value.toInt()) };
    } else if (value.canConvert(QVariant::Double)) {
        return { value.toDouble() };
    } else {
        return {};
    }
}

template <> optional<GeoJSON> ValueTraits<QVariant>::toGeoJSON(const QVariant& value, Error& error) {
#if QT_VERSION >= 0x050000
    if (value.typeName() == QStringLiteral("QMapbox::Feature")) {
#else
    if (value.typeName() == QString("QMapbox::Feature")) {
#endif
        return GeoJSON { asMapboxGLFeature(value.value<QMapbox::Feature>()) };
    } else if (value.type() != QVariant::ByteArray) {
        error = { "JSON data must be in QByteArray" };
        return {};
    }

    QByteArray data = value.toByteArray();
    return parseGeoJSON(std::string(data.constData(), data.size()), error);
}

} // namespace conversion
} // namespace style
} // namespace mbgl
