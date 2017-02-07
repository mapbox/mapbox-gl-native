#pragma once

#include <mbgl/style/filter.hpp>
#include <mbgl/style/property_value.hpp>
#include <mbgl/style/layout_property.hpp>
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
class StringifyFilter {
public:
    Writer& writer;

    void operator()(const NullFilter&) {
        writer.Null();
    }

    void operator()(const EqualsFilter& f) {
        stringifyBinaryFilter(f, "==");
    }

    void operator()(const NotEqualsFilter& f) {
        stringifyBinaryFilter(f, "!=");
    }

    void operator()(const LessThanFilter& f) {
        stringifyBinaryFilter(f, "<");
    }

    void operator()(const LessThanEqualsFilter& f) {
        stringifyBinaryFilter(f, "<=");
    }

    void operator()(const GreaterThanFilter& f) {
        stringifyBinaryFilter(f, ">");
    }

    void operator()(const GreaterThanEqualsFilter& f) {
        stringifyBinaryFilter(f, ">=");
    }

    void operator()(const InFilter& f) {
        stringifySetFilter(f, "in");
    }

    void operator()(const NotInFilter& f) {
        stringifySetFilter(f, "!in");
    }

    void operator()(const AllFilter& f) {
        stringifyCompoundFilter(f, "all");
    }

    void operator()(const AnyFilter& f) {
        stringifyCompoundFilter(f, "any");
    }

    void operator()(const NoneFilter& f) {
        stringifyCompoundFilter(f, "none");
    }

    void operator()(const HasFilter& f) {
        stringifyUnaryFilter(f, "has");
    }

    void operator()(const NotHasFilter& f) {
        stringifyUnaryFilter(f, "!has");
    }

private:
    template <class F>
    void stringifyBinaryFilter(const F& f, const char * op) {
        writer.StartArray();
        writer.String(op);
        writer.String(f.key);
        stringify(writer, f.value);
        writer.EndArray();
    }

    template <class F>
    void stringifySetFilter(const F& f, const char * op) {
        writer.StartArray();
        writer.String(op);
        writer.String(f.key);
        for (const auto& value : f.values) {
            stringify(writer, value);
        }
        writer.EndArray();
    }

    template <class F>
    void stringifyCompoundFilter(const F& f, const char * op) {
        writer.StartArray();
        writer.String(op);
        for (const auto& filter : f.filters) {
            Filter::visit(filter, *this);
        }
        writer.EndArray();
    }

    template <class F>
    void stringifyUnaryFilter(const F& f, const char * op) {
        writer.StartArray();
        writer.String(op);
        writer.String(f.key);
        writer.EndArray();
    }
};

template <class Writer>
void stringify(Writer& writer, const Filter& f) {
    Filter::visit(f, StringifyFilter<Writer> { writer });
}

template <class Writer>
void stringify(Writer& writer, const Undefined&) {
    assert(false); // Should be omitted entirely instead.
    writer.Null();
}

template <class Writer>
void stringify(Writer& writer, const CategoricalValue& v) {
    CategoricalValue::visit(v, [&] (const auto& v_) { stringify(writer, v_); });
}

template <class Writer>
class StringifyStops {
public:
    Writer& writer;

    template <class T>
    void operator()(const ExponentialStops<T>& f) {
        writer.Key("type");
        writer.String("exponential");
        writer.Key("base");
        writer.Double(f.base);
        writer.Key("stops");
        stringifyStops(f.stops);
    }

    template <class T>
    void operator()(const IntervalStops<T>& f) {
        writer.Key("type");
        writer.String("interval");
        writer.Key("stops");
        stringifyStops(f.stops);
    }

    template <class T>
    void operator()(const CategoricalStops<T>& f) {
        writer.Key("type");
        writer.String("categorical");
        writer.Key("stops");
        stringifyStops(f.stops);
    }

    template <class T>
    void operator()(const IdentityStops<T>&) {
        writer.Key("type");
        writer.String("identity");
    }

    template <class T>
    void operator()(const CompositeExponentialStops<T>& f) {
        writer.Key("type");
        writer.String("exponential");
        writer.Key("base");
        writer.Double(f.base);
        writer.Key("stops");
        stringifyCompositeStops(f.stops);
    }

    template <class T>
    void operator()(const CompositeIntervalStops<T>& f) {
        writer.Key("type");
        writer.String("interval");
        writer.Key("stops");
        stringifyCompositeStops(f.stops);
    }

    template <class T>
    void operator()(const CompositeCategoricalStops<T>& f) {
        writer.Key("type");
        writer.String("categorical");
        writer.Key("stops");
        stringifyCompositeStops(f.stops);
    }

private:
    template <class K, class V>
    void stringifyStops(const std::map<K, V>& stops) {
        writer.StartArray();
        for (const auto& stop : stops) {
            writer.StartArray();
            stringify(writer, stop.first);
            stringify(writer, stop.second);
            writer.EndArray();
        }
        writer.EndArray();
    }

    template <class InnerStops>
    void stringifyCompositeStops(const std::map<float, InnerStops>& stops) {
        writer.StartArray();
        for (const auto& outer : stops) {
            for (const auto& inner : outer.second) {
                writer.StartArray();
                writer.StartObject();
                writer.Key("zoom");
                writer.Double(outer.first);
                writer.Key("value");
                stringify(writer, inner.first);
                writer.EndObject();
                stringify(writer, inner.second);
                writer.EndArray();
            }
        }
        writer.EndArray();
    }
};

template <class Writer, class T>
void stringify(Writer& writer, const CameraFunction<T>& f) {
    writer.StartObject();
    CameraFunction<T>::Stops::visit(f.stops, StringifyStops<Writer> { writer });
    writer.EndObject();
}

template <class Writer, class T>
void stringify(Writer& writer, const SourceFunction<T>& f) {
    writer.StartObject();
    writer.Key("property");
    writer.String(f.property);
    SourceFunction<T>::Stops::visit(f.stops, StringifyStops<Writer> { writer });
    if (f.defaultValue) {
        writer.Key("default");
        stringify(writer, *f.defaultValue);
    }
    writer.EndObject();
}

template <class Writer, class T>
void stringify(Writer& writer, const CompositeFunction<T>& f) {
    writer.StartObject();
    writer.Key("property");
    writer.String(f.property);
    CompositeFunction<T>::Stops::visit(f.stops, StringifyStops<Writer> { writer });
    if (f.defaultValue) {
        writer.Key("default");
        stringify(writer, *f.defaultValue);
    }
    writer.EndObject();
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

template <class Writer, class... Ps>
void stringify(Writer& writer, const LayoutProperties<Ps...>& ps) {
    writer.StartObject();
    util::ignore({ (stringify<Ps>(writer, ps.unevaluated.template get<Ps>()), 0)... });
    writer.EndObject();
}

} // namespace conversion
} // namespace style
} // namespace mbgl
