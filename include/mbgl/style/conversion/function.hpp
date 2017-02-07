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

template <class D, class R, class V>
Result<std::map<D, R>> convertStops(const V& value) {
    auto stopsValue = objectMember(value, "stops");
    if (!stopsValue) {
        return Error { "function value must specify stops" };
    }

    if (!isArray(*stopsValue)) {
        return Error { "function stops must be an array" };
    }

    if (arrayLength(*stopsValue) == 0) {
        return Error { "function must have at least one stop" };
    }

    std::map<D, R> stops;
    for (std::size_t i = 0; i < arrayLength(*stopsValue); ++i) {
        const auto& stopValue = arrayMember(*stopsValue, i);

        if (!isArray(stopValue)) {
            return Error { "function stop must be an array" };
        }

        if (arrayLength(stopValue) != 2) {
            return Error { "function stop must have two elements" };
        }

        Result<D> d = convert<D>(arrayMember(stopValue, 0));
        if (!d) {
            return d.error();
        }

        Result<R> r = convert<R>(arrayMember(stopValue, 1));
        if (!r) {
            return r.error();
        }

        stops.emplace(*d, *r);
    }

    return stops;
}

template <class T>
struct Converter<ExponentialStops<T>> {
    static constexpr const char * type = "exponential";

    template <class V>
    Result<ExponentialStops<T>> operator()(const V& value) const {
        auto stops = convertStops<float, T>(value);
        if (!stops) {
            return stops.error();
        }

        auto baseValue = objectMember(value, "base");
        if (!baseValue) {
            return ExponentialStops<T>(*stops);
        }

        optional<float> base = toNumber(*baseValue);
        if (!base) {
            return Error { "function base must be a number"};
        }

        return ExponentialStops<T>(*stops, *base);
    }
};

template <class T>
struct Converter<IntervalStops<T>> {
    static constexpr const char * type = "interval";

    template <class V>
    Result<IntervalStops<T>> operator()(const V& value) const {
        auto stops = convertStops<float, T>(value);
        if (!stops) {
            return stops.error();
        }
        return IntervalStops<T>(*stops);
    }
};

template <>
struct Converter<CategoricalValue> {
    template <class V>
    Result<CategoricalValue> operator()(const V& value) const {
        auto b = toBool(value);
        if (b) {
            return *b;
        }

        auto n = toNumber(value);
        if (n) {
            return int64_t(*n);
        }

        auto s = toString(value);
        if (s) {
            return *s;
        }

        return Error { "stop domain value must be a number, string, or boolean" };
    }
};

template <class T>
struct Converter<CategoricalStops<T>> {
    static constexpr const char * type = "categorical";

    template <class V>
    Result<CategoricalStops<T>> operator()(const V& value) const {
        auto stops = convertStops<CategoricalValue, T>(value);
        if (!stops) {
            return stops.error();
        }
        return CategoricalStops<T>(
            std::map<CategoricalValue, T>((*stops).begin(), (*stops).end()));
    }
};

template <class T>
struct Converter<IdentityStops<T>> {
    static constexpr const char * type = "identity";

    template <class V>
    Result<IdentityStops<T>> operator()(const V&) const {
        return IdentityStops<T>();
    }
};

template <class, class>
struct StopsConverter;

template <class T, class... Ts>
struct StopsConverter<T, variant<Ts...>> {
public:
    template <class V>
    Result<variant<Ts...>> operator()(const V& value) const {
        std::string type = util::Interpolatable<T> ? "exponential" : "interval";

        auto typeValue = objectMember(value, "type");
        if (typeValue && toString(*typeValue)) {
            type = *toString(*typeValue);
        }

        optional<Result<variant<Ts...>>> result;

        // Workaround for https://gcc.gnu.org/bugzilla/show_bug.cgi?id=47226
        auto tryConvert = [&] (auto* tp) {
            using Stops = std::decay_t<decltype(*tp)>;
            if (type == Converter<Stops>::type) {
                auto stops = convert<Stops>(value);
                result = stops
                    ? Result<variant<Ts...>>(*stops)
                    : Result<variant<Ts...>>(stops.error());
            }
        };

        util::ignore({
            (tryConvert((Ts*)nullptr), 0)...
        });

        if (!result) {
            return Error { "unsupported function type" };
        }

        return *result;
    }
};

template <class T>
struct Converter<CameraFunction<T>> {
    template <class V>
    Result<CameraFunction<T>> operator()(const V& value) const {
        if (!isObject(value)) {
            return Error { "function must be an object" };
        }

        auto stops = StopsConverter<T, typename CameraFunction<T>::Stops>()(value);
        if (!stops) {
            return stops.error();
        }

        return CameraFunction<T>(*stops);
    }
};

template <class T, class V>
Result<optional<T>> convertDefaultValue(const V& value) {
    auto defaultValueValue = objectMember(value, "defaultValue");
    if (!defaultValueValue) {
        return {};
    }

    auto defaultValue = convert<T>(*defaultValueValue);
    if (!defaultValue) {
        return Error { "wrong type for \"default\": " + defaultValue.error().message };
    }

    return *defaultValue;
}

template <class T>
struct Converter<SourceFunction<T>> {
    template <class V>
    Result<SourceFunction<T>> operator()(const V& value) const {
        if (!isObject(value)) {
            return Error { "function must be an object" };
        }

        auto propertyValue = objectMember(value, "property");
        if (!propertyValue) {
            return Error { "function must specify property" };
        }

        auto propertyString = toString(*propertyValue);
        if (!propertyString) {
            return Error { "function property must be a string" };
        }

        auto stops = StopsConverter<T, typename SourceFunction<T>::Stops>()(value);
        if (!stops) {
            return stops.error();
        }

        auto defaultValue = convertDefaultValue<T>(value);
        if (!defaultValue) {
            return defaultValue.error();
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
    template <class V>
    Result<CompositeValue<S>> operator()(const V& value) const {
        if (!isObject(value)) {
            return Error { "stop must be an object" };
        }

        auto zoomValue = objectMember(value, "zoom");
        if (!zoomValue) {
            return Error { "stop must specify zoom" };
        }

        auto propertyValue = objectMember(value, "value");
        if (!propertyValue) {
            return Error { "stop must specify value" };
        }

        Result<float> z = convert<float>(*zoomValue);
        if (!z) {
            return z.error();
        }

        Result<S> s = convert<S>(*propertyValue);
        if (!s) {
            return s.error();
        }

        return CompositeValue<S> { *z, *s };
    }
};

template <class T>
struct Converter<CompositeExponentialStops<T>> {
    static constexpr const char * type = "exponential";

    template <class V>
    Result<CompositeExponentialStops<T>> operator()(const V& value) const {
        auto stops = convertStops<CompositeValue<float>, T>(value);
        if (!stops) {
            return stops.error();
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

    template <class V>
    Result<CompositeIntervalStops<T>> operator()(const V& value) const {
        auto stops = convertStops<CompositeValue<float>, T>(value);
        if (!stops) {
            return stops.error();
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

    template <class V>
    Result<CompositeCategoricalStops<T>> operator()(const V& value) const {
        auto stops = convertStops<CompositeValue<CategoricalValue>, T>(value);
        if (!stops) {
            return stops.error();
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
    template <class V>
    Result<CompositeFunction<T>> operator()(const V& value) const {
        if (!isObject(value)) {
            return Error { "function must be an object" };
        }

        auto propertyValue = objectMember(value, "property");
        if (!propertyValue) {
            return Error { "function must specify property" };
        }

        auto propertyString = toString(*propertyValue);
        if (!propertyString) {
            return Error { "function property must be a string" };
        }

        auto stops = StopsConverter<T, typename CompositeFunction<T>::Stops>()(value);
        if (!stops) {
            return stops.error();
        }

        auto defaultValue = convertDefaultValue<T>(value);
        if (!defaultValue) {
            return defaultValue.error();
        }

        return CompositeFunction<T>(*propertyString, *stops, *defaultValue);
    }
};

} // namespace conversion
} // namespace style
} // namespace mbgl
