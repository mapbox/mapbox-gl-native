#pragma once

#include <mbgl/style/layer.hpp>
#include <mbgl/style/types.hpp>
#include <mbgl/style/conversion.hpp>
#include <mbgl/style/conversion/constant.hpp>
#include <mbgl/style/conversion/property_value.hpp>
#include <mbgl/util/color.hpp>
#include <mbgl/util/enum.hpp>

#include <functional>
#include <string>
#include <utility>

#include <QColor>
#include <QVariant>

namespace mbgl {
namespace style {
namespace conversion {

using QtLayoutPropertySetter = std::function<optional<Error> (Layer &, const QVariant &)>;
using QtPaintPropertySetter = std::function<optional<Error> (Layer &, const QVariant &, const optional<std::string> &)>;

template <typename T, class Enable = void>
struct FromQMapboxPropertyValueConverter;

template <>
struct FromQMapboxPropertyValueConverter<bool> {
    bool operator()(const QVariant &value) {
        return value.toBool();
    }
};

template <>
struct FromQMapboxPropertyValueConverter<float> {
    float operator()(const QVariant &value) {
        return value.toFloat();
    }
};

template <>
struct FromQMapboxPropertyValueConverter<std::string> {
    std::string operator()(const QVariant &value) {
        return value.toString().toStdString();
    }
};

template <typename T>
struct FromQMapboxPropertyValueConverter<T, typename std::enable_if_t<std::is_enum<T>::value>> {
    T operator()(const QVariant &value) {
        std::string string = value.toString().toStdString();
        return *Enum<T>::toEnum(string);
    }
};

template <>
struct FromQMapboxPropertyValueConverter<Color> {
    Color operator()(const QVariant &value) {
        QColor color = value.value<QColor>();
        return { float(color.redF()), float(color.greenF()), float(color.blueF()), float(color.alphaF()) };
    }
};

template <>
struct FromQMapboxPropertyValueConverter<std::array<float, 2>> {
    std::array<float, 2> operator()(const QVariant &value) {
        QMapbox::PropertyValueNumbers numbers = value.value<QMapbox::PropertyValueNumbers>();
        return {{ numbers[0], numbers[1] }};
    }
};

template <>
struct FromQMapboxPropertyValueConverter<std::array<float, 4>> {
    std::array<float, 4> operator()(const QVariant &value) {
        QMapbox::PropertyValueNumbers numbers = value.value<QMapbox::PropertyValueNumbers>();
        return {{ numbers[0], numbers[1], numbers[2], numbers[3] }};
    }
};

template <>
struct FromQMapboxPropertyValueConverter<std::vector<float>> {
    std::vector<float> operator()(const QVariant &value) {
        QMapbox::PropertyValueNumbers numbers = value.value<QMapbox::PropertyValueNumbers>();
        std::vector<float> mbglNumbers;
        for (const auto number : numbers) {
            mbglNumbers.emplace_back(number);
        }
        return mbglNumbers;
    }
};

template <>
struct FromQMapboxPropertyValueConverter<std::vector<std::string>> {
    std::vector<std::string> operator()(const QVariant &value) {
        QMapbox::PropertyValueStrings strings = value.value<QMapbox::PropertyValueStrings>();
        std::vector<std::string> mbglStrings;
        for (const auto string : strings) {
            mbglStrings.emplace_back(string.toStdString());
        }
        return mbglStrings;
    }
};

template <class L, class T, class...Args>
auto makeQtPropertySetter(void (L::*setter)(PropertyValue<T>, const Args&...args)) {
    return [setter] (Layer &layer, const QVariant &value, const Args&...args) -> optional<Error> {
        L* typedLayer = layer.as<L>();
        if (!typedLayer) {
            return Error { "layer doesn't support this property" };
        }

        Result<PropertyValue<T>> typedValue = [&value]() -> Result<PropertyValue<T>> {
            Q_UNUSED(value);

            if (value.isNull()) {
                return {};
            } else if (value.canConvert<QMapbox::PropertyValueFunction>()) {
                QMapbox::PropertyValueFunction function = value.value<QMapbox::PropertyValueFunction>();
                std::vector<std::pair<float, T>> mbglStops;
                for (const auto& stop : function.stops) {
                    mbglStops.emplace_back(std::make_pair<float, T>(float(stop.first), FromQMapboxPropertyValueConverter<T>()(stop.second)));
                }
                return Function<T>(mbglStops, function.base);
            }

            return FromQMapboxPropertyValueConverter<T>()(value);
        }();

        if (!typedValue) {
            return typedValue.error();
        }

        (typedLayer->*setter)(*typedValue, args...);
        return {};
    };
}

optional<Error> setQtVisibility(Layer &layer, const QVariant &value) {
    layer.setVisibility(static_cast<VisibilityType>(value.toBool()));
    return {};
}

} // namespace conversion
} // namespace style
} // namespace mbgl
