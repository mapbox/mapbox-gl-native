#pragma once

#include <mbgl/style/conversion.hpp>
#include <mbgl/util/feature.hpp>
#include <mbgl/util/optional.hpp>

#include <QColor>
#include <QVariant>

namespace mbgl {
namespace style {
namespace conversion {

inline bool isUndefined(const QVariant& value) {
    return value.isNull() || !value.isValid();
}

inline bool isArray(const QVariant& value) {
    return value.canConvert(QVariant::List);
}

inline std::size_t arrayLength(const QVariant& value) {
    return value.toList().size();
}

inline QVariant arrayMember(const QVariant& value, std::size_t i) {
    return value.toList()[i];
}

inline bool isObject(const QVariant& value) {
    return value.canConvert(QVariant::Map) || value.type() == QVariant::ByteArray;
}

inline optional<QVariant> objectMember(const QVariant& value, const char* key) {
    auto map = value.toMap();
    auto iter = map.constFind(key);

    if (iter != map.constEnd()) {
        return iter.value();
    } else {
        return {};
    }
}

using EachMemberFn = std::function<optional<Error>(const std::string&, const QVariant&)>;

optional<Error> eachMember(const QVariant& value, EachMemberFn&& fn) {
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

inline optional<bool> toBool(const QVariant& value) {
    if (value.type() == QVariant::Bool) {
        return value.toBool();
    } else {
        return {};
    }
}

inline optional<float> toNumber(const QVariant& value) {
    if (value.type() == QVariant::Double) {
        return value.toFloat();
    } else {
        return {};
    }
}

inline optional<std::string> toString(const QVariant& value) {
    if (value.type() == QVariant::String) {
        return value.toString().toStdString();
    } else if (value.type() == QVariant::Color) {
        return value.value<QColor>().name().toStdString();
    } else {
        return {};
    }
}

inline optional<Value> toValue(const QVariant& value) {
    if (value.type() == QVariant::Bool) {
        return { value.toBool() };
    } else if (value.type() == QVariant::String) {
        return { value.toString().toStdString() };
    } else if (value.type() == QVariant::Color) {
        return { value.value<QColor>().name().toStdString() };
    } else if (value.type() == QVariant::Int) {
        return { value.toInt() };
    } else if (value.canConvert(QVariant::Double)) {
        return { value.toFloat() };
    } else {
        return {};
    }
}

} // namespace conversion
} // namespace style
} // namespace mbgl
