#pragma once

#include <mbgl/style/filter.hpp>
#include <mbgl/style/property_value.hpp>
#include <mbgl/style/data_driven_property_value.hpp>
#include <mbgl/util/enum.hpp>
#include <mbgl/util/color.hpp>
#include <mbgl/util/feature.hpp>
#include <mbgl/util/ignore.hpp>
#include <mbgl/util/rapidjson.hpp>

#include <array>
#include <vector>
#include <unordered_map>

namespace mbgl {
namespace style {
namespace conversion {

template <class Writer>
void stringify(Writer& writer, NullValue) {
    writer.Null();
}

template <class Writer>
void stringify(Writer& writer, bool v) {
    writer.Bool(v);
}

template <class Writer>
void stringify(Writer& writer, uint64_t v) {
    writer.Uint64(v);
}

template <class Writer>
void stringify(Writer& writer, int64_t v) {
    writer.Int64(v);
}

template <class Writer>
void stringify(Writer& writer, double v) {
    writer.Double(v);
}

template <class Writer>
void stringify(Writer& writer, const std::string& v) {
    writer.String(v);
}

template <class Writer, class T, class Enable = std::enable_if_t<std::is_enum<T>::value>>
void stringify(Writer& writer, const T& v) {
    writer.String(Enum<T>::toString(v));
}

template <class Writer>
void stringify(Writer& writer, const Color& v) {
    writer.String(v.stringify());
}

template <class Writer>
void stringify(Writer& writer, const std::array<float, 2>& v) {
    writer.StartArray();
    writer.Double(v[0]);
    writer.Double(v[1]);
    writer.EndArray();
}

template <class Writer>
void stringify(Writer& writer, const std::array<float, 4>& v) {
    writer.StartArray();
    writer.Double(v[0]);
    writer.Double(v[1]);
    writer.Double(v[2]);
    writer.Double(v[3]);
    writer.EndArray();
}

template <class Writer>
void stringify(Writer&, const Value&);

template <class Writer, class T>
void stringify(Writer& writer, const std::vector<T>& v) {
    writer.StartArray();
    for (const auto& e : v) {
        stringify(writer, e);
    }
    writer.EndArray();
}

template <class Writer, class T>
void stringify(Writer& writer, const std::unordered_map<std::string, T>& m) {
    writer.StartObject();
    for (const auto& p : m) {
        writer.Key(p.first.data(), static_cast<unsigned>(p.first.size()));
        stringify(writer, p.second);
    }
    writer.EndObject();
}

template <class Writer>
void stringify(Writer& writer, const Value& v) {
    Value::visit(v, [&] (const auto& v_) { stringify(writer, v_); });
}

template <class Writer>
void stringify(Writer& writer, FeatureType type) {
    switch (type) {
    case FeatureType::Unknown:
        writer.String("Unknown");
        break;
    case FeatureType::Point:
        writer.String("Point");
        break;
    case FeatureType::LineString:
        writer.String("LineString");
        break;
    case FeatureType::Polygon:
        writer.String("Polygon");
        break;
    }
}

template <class Writer>
void stringify(Writer& writer, const FeatureIdentifier& id) {
    FeatureIdentifier::visit(id, [&] (const auto& id_) { stringify(writer, id_); });
}

template <class Writer>
void stringify(Writer& writer, const Filter& filter) {
    if (!filter.expression) writer.Null();
    else stringify(writer, (*filter.expression)->serialize());
}

template <class Writer>
void stringify(Writer& writer, const Undefined&) {
    assert(false); // Should be omitted entirely instead.
    writer.Null();
}

template <class Writer, class T>
void stringify(Writer& writer, const PropertyExpression<T>& fn) {
    stringify(writer, fn.getExpression().serialize());
}

template <class Writer, class T>
void stringify(Writer& writer, const PropertyValue<T>& v) {
    v.evaluate([&] (const auto& v_) { stringify(writer, v_); });
}

template <class Property, class Writer, class T>
void stringify(Writer& writer, const PropertyValue<T>& value) {
    if (!value.isUndefined()) {
        writer.Key(Property::key);
        stringify(writer, value);
    }
}

template <class Writer, class T>
void stringify(Writer& writer, const DataDrivenPropertyValue<T>& v) {
    v.evaluate([&] (const auto& v_) { stringify(writer, v_); });
}

template <class Property, class Writer, class T>
void stringify(Writer& writer, const DataDrivenPropertyValue<T>& value) {
    if (!value.isUndefined()) {
        writer.Key(Property::key);
        stringify(writer, value);
    }
}

} // namespace conversion
} // namespace style
} // namespace mbgl
