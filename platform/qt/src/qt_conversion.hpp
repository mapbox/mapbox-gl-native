#pragma once

#include <mbgl/style/conversion.hpp>
#include <mbgl/style/conversion/geojson.hpp>
#include <mbgl/util/optional.hpp>

#include <QVariant>
#include <QColor>
#include <QMapbox>
#include "qt_geojson.hpp"

namespace mbgl {
namespace style {
namespace conversion {

template <>
class ConversionTraits<QVariant> {
public:
    static bool isUndefined(const QVariant& value) {
        return value.isNull() || !value.isValid();
    }

    static bool isArray(const QVariant& value) {
        return value.canConvert(QVariant::List);
    }

    static std::size_t arrayLength(const QVariant& value) {
        return value.toList().size();
    }

    static QVariant arrayMember(const QVariant& value, std::size_t i) {
        return value.toList()[i];
    }

    static bool isObject(const QVariant& value) {
        return value.canConvert(QVariant::Map)
            || value.type() == QVariant::ByteArray
    #if QT_VERSION >= 0x050000
            || QString(value.typeName()) == QStringLiteral("QMapbox::Feature");
    #else
            || QString(value.typeName()) == QString("QMapbox::Feature");
    #endif
    }

    static optional<QVariant> objectMember(const QVariant& value, const char* key) {
        auto map = value.toMap();
        auto iter = map.constFind(key);

        if (iter != map.constEnd()) {
            return iter.value();
        } else {
            return {};
        }
    }

    template <class Fn>
    static optional<Error> eachMember(const QVariant& value, Fn&& fn) {
        auto map = value.toMap();
        auto iter = map.constBegin();

        while (iter != map.constEnd()) {
            optional<Error> result = fn(iter.key().toStdString(), QVariant(iter.value()));
            if (result) {
                return result;
            }

            ++iter;
        }

        return {};
    }

    static optional<bool> toBool(const QVariant& value) {
        if (value.type() == QVariant::Bool) {
            return value.toBool();
        } else {
            return {};
        }
    }

    static optional<float> toNumber(const QVariant& value) {
        if (value.type() == QVariant::Int || value.type() == QVariant::Double) {
            return value.toFloat();
        } else {
            return {};
        }
    }

    static optional<double> toDouble(const QVariant& value) {
        if (value.type() == QVariant::Int || value.type() == QVariant::Double) {
            return value.toDouble();
        } else {
            return {};
        }
    }

    static optional<std::string> toString(const QVariant& value) {
        if (value.type() == QVariant::String) {
            return value.toString().toStdString();
        } else if (value.type() == QVariant::Color) {
            return value.value<QColor>().name().toStdString();
        } else {
            return {};
        }
    }

    static optional<Value> toValue(const QVariant& value) {
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

    static optional<GeoJSON> toGeoJSON(const QVariant& value, Error& error) {
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
};

template <class T, class...Args>
optional<T> convert(const QVariant& value, Error& error, Args&&...args) {
    return convert<T>(Convertible(value), error, std::forward<Args>(args)...);
}

} // namespace conversion
} // namespace style
} // namespace mbgl
