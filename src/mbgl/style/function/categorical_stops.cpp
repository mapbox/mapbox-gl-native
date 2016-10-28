#include <mbgl/style/function/categorical_stops.hpp>
#include <mbgl/util/color.hpp>

#include <array>

namespace mbgl {
namespace style {

optional<CategoricalValue> categoricalValue(const Value& value) {
    return value.match(
        [] (bool t) { return optional<CategoricalValue>(t); },
        [] (uint64_t t) { return optional<CategoricalValue>(int64_t(t)); },
        [] (int64_t t) { return optional<CategoricalValue>(t); },
        [] (double t) { return optional<CategoricalValue>(int64_t(t)); },
        [] (const std::string& t) { return optional<CategoricalValue>(t); },
        [] (const auto&) { return optional<CategoricalValue>(); }
    );
}

template <class T>
T CategoricalStops<T>::evaluate(const Value& value) const {
    auto v = categoricalValue(value);
    if (!v) {
        return defaultValue;
    }
    auto it = stops.find(*v);
    return it == stops.end() ? defaultValue : it->second;
}

template class CategoricalStops<float>;
template class CategoricalStops<Color>;
template class CategoricalStops<std::array<float, 2>>;

} // namespace style
} // namespace mbgl
