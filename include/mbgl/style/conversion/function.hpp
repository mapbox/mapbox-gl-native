#pragma once

#include <mbgl/style/function/camera_function.hpp>
#include <mbgl/style/function/source_function.hpp>
#include <mbgl/style/function/composite_function.hpp>
#include <mbgl/style/conversion.hpp>
#include <mbgl/style/conversion/constant.hpp>
#include <mbgl/util/ignore.hpp>

namespace mbgl {
namespace style {
namespace conversion {

template <class D, class R>
optional<std::map<D, R>> convertStops(const Convertible& value, Error& error) {
    auto stopsValue = objectMember(value, "stops");
    if (!stopsValue) {
        error = { "function value must specify stops" };
        return {};
    }

    if (!isArray(*stopsValue)) {
        error = { "function stops must be an array" };
        return {};
    }

    if (arrayLength(*stopsValue) == 0) {
        error = { "function must have at least one stop" };
        return {};
    }

    std::map<D, R> stops;
    for (std::size_t i = 0; i < arrayLength(*stopsValue); ++i) {
        const auto& stopValue = arrayMember(*stopsValue, i);

        if (!isArray(stopValue)) {
            error = { "function stop must be an array" };
            return {};
        }

        if (arrayLength(stopValue) != 2) {
            error = { "function stop must have two elements" };
            return {};
        }

        optional<D> d = convert<D>(arrayMember(stopValue, 0), error);
        if (!d) {
            return {};
        }

        optional<R> r = convert<R>(arrayMember(stopValue, 1), error);
        if (!r) {
            return {};
        }

        stops.emplace(*d, *r);
    }

    return stops;
}

template <class T>
struct Converter<ExponentialStops<T>> {
    static constexpr const char * type = "exponential";

    optional<ExponentialStops<T>> operator()(const Convertible& value, Error& error) const {
        auto stops = convertStops<float, T>(value, error);
        if (!stops) {
            return {};
        }

        auto baseValue = objectMember(value, "base");
        if (!baseValue) {
            return ExponentialStops<T>(*stops);
        }

        optional<float> base = toNumber(*baseValue);
        if (!base) {
            error = { "function base must be a number"};
            return {};
        }

        return ExponentialStops<T>(*stops, *base);
    }
};

template <class T>
struct Converter<IntervalStops<T>> {
    static constexpr const char * type = "interval";

    optional<IntervalStops<T>> operator()(const Convertible& value, Error& error) const {
        auto stops = convertStops<float, T>(value, error);
        if (!stops) {
            return {};
        }
        return IntervalStops<T>(*stops);
    }
};

template <>
struct Converter<CategoricalValue> {
    optional<CategoricalValue> operator()(const Convertible& value, Error& error) const {
        auto b = toBool(value);
        if (b) {
            return { *b };
        }

        auto n = toNumber(value);
        if (n) {
            return { int64_t(*n) };
        }

        auto s = toString(value);
        if (s) {
            return { *s };
        }

        error = { "stop domain value must be a number, string, or boolean" };
        return {};
    }
};

template <class T>
struct Converter<CategoricalStops<T>> {
    static constexpr const char * type = "categorical";

    optional<CategoricalStops<T>> operator()(const Convertible& value, Error& error) const {
        auto stops = convertStops<CategoricalValue, T>(value, error);
        if (!stops) {
            return {};
        }
        return CategoricalStops<T>(
            std::map<CategoricalValue, T>((*stops).begin(), (*stops).end()));
    }
};

template <class T>
struct Converter<IdentityStops<T>> {
    static constexpr const char * type = "identity";

    optional<IdentityStops<T>> operator()(const Convertible&, Error&) const {
        return IdentityStops<T>();
    }
};

template <class, class>
struct StopsConverter;

template <class T, class... Ts>
struct StopsConverter<T, variant<Ts...>> {
public:
    optional<variant<Ts...>> operator()(const Convertible& value, Error& error) const {
        std::string type = util::Interpolatable<T>::value ? "exponential" : "interval";

        auto typeValue = objectMember(value, "type");
        if (typeValue && toString(*typeValue)) {
            type = *toString(*typeValue);
        }

        bool matched = false;
        optional<variant<Ts...>> result;

        // Workaround for https://gcc.gnu.org/bugzilla/show_bug.cgi?id=47226
        auto tryConvert = [&] (auto* tp) {
            using Stops = std::decay_t<decltype(*tp)>;
            if (type == Converter<Stops>::type) {
                matched = true;
                optional<Stops> stops = convert<Stops>(value, error);
                if (stops) {
                    result = variant<Ts...>(*stops);
                }
            }
        };

        util::ignore({
            (tryConvert((Ts*)nullptr), 0)...
        });

        if (!matched) {
            error = { "unsupported function type" };
            return {};
        }

        return result;
    }
};

template <class T>
struct Converter<CameraFunction<T>> {
    optional<CameraFunction<T>> operator()(const Convertible& value, Error& error) const {
        if (!isObject(value)) {
            error = { "function must be an object" };
            return {};
        }

        auto stops = StopsConverter<T, typename CameraFunction<T>::Stops>()(value, error);
        if (!stops) {
            return {};
        }

        return CameraFunction<T>(*stops);
    }
};

template <class T>
optional<optional<T>> convertDefaultValue(const Convertible& value, Error& error) {
    auto defaultValueValue = objectMember(value, "default");
    if (!defaultValueValue) {
        return optional<T>();
    }

    auto defaultValue = convert<T>(*defaultValueValue, error);
    if (!defaultValue) {
        error = { R"(wrong type for "default": )" + error.message };
        return {};
    }

    return { *defaultValue };
}

template <class T>
struct Converter<SourceFunction<T>> {
    optional<SourceFunction<T>> operator()(const Convertible& value, Error& error) const {
        if (!isObject(value)) {
            error = { "function must be an object" };
            return {};
        }

        auto propertyValue = objectMember(value, "property");
        if (!propertyValue) {
            error = { "function must specify property" };
            return {};
        }

        auto propertyString = toString(*propertyValue);
        if (!propertyString) {
            error = { "function property must be a string" };
            return {};
        }

        auto stops = StopsConverter<T, typename SourceFunction<T>::Stops>()(value, error);
        if (!stops) {
            return {};
        }

        auto defaultValue = convertDefaultValue<T>(value, error);
        if (!defaultValue) {
            return {};
        }

        return SourceFunction<T>(*propertyString, *stops, *defaultValue);
    }
};

template <class S>
struct CompositeValue : std::pair<float, S> {
    using std::pair<float, S>::pair;
};

template <class S>
struct Converter<CompositeValue<S>> {
    optional<CompositeValue<S>> operator()(const Convertible& value, Error& error) const {
        if (!isObject(value)) {
            error = { "stop must be an object" };
            return {};
        }

        auto zoomValue = objectMember(value, "zoom");
        if (!zoomValue) {
            error = { "stop must specify zoom" };
            return {};
        }

        auto propertyValue = objectMember(value, "value");
        if (!propertyValue) {
            error = { "stop must specify value" };
            return {};
        }

        optional<float> z = convert<float>(*zoomValue, error);
        if (!z) {
            return {};
        }

        optional<S> s = convert<S>(*propertyValue, error);
        if (!s) {
            return {};
        }

        return CompositeValue<S> { *z, *s };
    }
};

template <class T>
struct Converter<CompositeExponentialStops<T>> {
    static constexpr const char * type = "exponential";

    optional<CompositeExponentialStops<T>> operator()(const Convertible& value, Error& error) const {
        auto stops = convertStops<CompositeValue<float>, T>(value, error);
        if (!stops) {
            return {};
        }

        auto base = 1.0f;
        auto baseValue = objectMember(value, "base");
        if (baseValue && toNumber(*baseValue)) {
            base = *toNumber(*baseValue);
        }

        std::map<float, std::map<float, T>> convertedStops;
        for (const auto& stop : *stops) {
            convertedStops[stop.first.first].emplace(stop.first.second, stop.second);
        }

        return CompositeExponentialStops<T>(convertedStops, base);
    }
};

template <class T>
struct Converter<CompositeIntervalStops<T>> {
    static constexpr const char * type = "interval";

    optional<CompositeIntervalStops<T>> operator()(const Convertible& value, Error& error) const {
        auto stops = convertStops<CompositeValue<float>, T>(value, error);
        if (!stops) {
            return {};
        }

        std::map<float, std::map<float, T>> convertedStops;
        for (const auto& stop : *stops) {
            convertedStops[stop.first.first].emplace(stop.first.second, stop.second);
        }

        return CompositeIntervalStops<T>(convertedStops);
    }
};

template <class T>
struct Converter<CompositeCategoricalStops<T>> {
    static constexpr const char * type = "categorical";

    optional<CompositeCategoricalStops<T>> operator()(const Convertible& value, Error& error) const {
        auto stops = convertStops<CompositeValue<CategoricalValue>, T>(value, error);
        if (!stops) {
            return {};
        }

        std::map<float, std::map<CategoricalValue, T>> convertedStops;
        for (const auto& stop : *stops) {
            convertedStops[stop.first.first].emplace(stop.first.second, stop.second);
        }

        return CompositeCategoricalStops<T>(convertedStops);
    }
};

template <class T>
struct Converter<CompositeFunction<T>> {
    optional<CompositeFunction<T>> operator()(const Convertible& value, Error& error) const {
        if (!isObject(value)) {
            error = { "function must be an object" };
            return {};
        }

        auto propertyValue = objectMember(value, "property");
        if (!propertyValue) {
            error = { "function must specify property" };
            return {};
        }

        auto propertyString = toString(*propertyValue);
        if (!propertyString) {
            error = { "function property must be a string" };
            return {};
        }

        auto stops = StopsConverter<T, typename CompositeFunction<T>::Stops>()(value, error);
        if (!stops) {
            return {};
        }

        auto defaultValue = convertDefaultValue<T>(value, error);
        if (!defaultValue) {
            return {};
        }

        return CompositeFunction<T>(*propertyString, *stops, *defaultValue);
    }
};

} // namespace conversion
} // namespace style
} // namespace mbgl
