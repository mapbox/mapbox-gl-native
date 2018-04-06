#pragma once

#include <mbgl/style/expression/type.hpp>
#include <mbgl/style/position.hpp>
#include <mbgl/style/types.hpp>
#include <mbgl/util/color.hpp>
#include <mbgl/util/enum.hpp>
#include <mbgl/util/feature.hpp>
#include <mbgl/util/variant.hpp>

#include <array>
#include <vector>

namespace mbgl {
namespace style {
namespace expression {

struct Value;

typedef variant<
    NullValue,
    bool,
    double,
    std::string,
    Color,
    mapbox::util::recursive_wrapper<std::vector<Value>>,
    mapbox::util::recursive_wrapper<std::unordered_map<std::string, Value>>> ValueBase;

class Value : public ValueBase {
public:
    using ValueBase::ValueBase;

    template <typename T>
    bool is(T*) const {
        return ValueBase::is<T>();
    }

    template <>
    bool is(float*) const {
        return ValueBase::is<double>();
    }

    template <typename T>
    bool is() const {
        return ValueBase::is<T>();
    }
    template <typename T>
    T& get(T*) {
        return ValueBase::get<T>();
    }
    template <typename T>
    T const& get(T*) const {
        return ValueBase::get<T>();
    }
    template <>
    float const& get(float*) const {
        return ValueBase::get<double>();
    }


    template <typename T>
    T& get() {
        return ValueBase::get<T>();
    }
    template <typename T>
    T const& get() const {
        return ValueBase::get<T>();
    }

    Value(NullValue&& args)
    : ValueBase(std::move(args))
    { }
    Value(bool&& args)
    : ValueBase(std::forward<bool>(args))
    { }
    Value(double&& args)
    : ValueBase(std::forward<double>(args))
    { }
    Value(std::string&& args)
    : ValueBase(std::move(args))
    { }
    Value(Color&& args)
    : ValueBase(std::forward<Color>(args))
    { }
    Value(std::vector<Value>&& args)
    : ValueBase(mapbox::util::recursive_wrapper<std::vector<Value>>(args))
    { }
    typedef mapbox::util::recursive_wrapper<std::unordered_map<std::string, Value>> rec_map;
    Value(std::unordered_map<std::string, Value>&& args)
    : ValueBase(rec_map(args))
    { }

    Value(const NullValue& args)
    : ValueBase(args)
    { }
    Value(const bool& args)
    : ValueBase(args)
    { }
    Value(const double& args)
    : ValueBase(args)
    { }
    Value(const std::string& args)
    : ValueBase(args)
    { }
    Value(const Color& args)
    : ValueBase(args)
    { }
    Value(const std::vector<Value>& args)
    : ValueBase(mapbox::util::recursive_wrapper<std::vector<Value>>(args))
    { }
    Value(const std::unordered_map<std::string, Value>& args)
    : ValueBase(rec_map(args))
    { }


    Value(const Value&) = default;
    
    // Javascript's Number.MAX_SAFE_INTEGER
    static uint64_t maxSafeInteger() { return 9007199254740991ULL; }
    
    static bool isSafeInteger(uint64_t x) { return x <= maxSafeInteger(); };
    static bool isSafeInteger(int64_t x) {
        return static_cast<uint64_t>(x > 0 ? x : -x) <= maxSafeInteger();
    }
    static bool isSafeInteger(double x) {
        return static_cast<uint64_t>(x > 0 ? x : -x) <= maxSafeInteger();
    }
    
};

constexpr NullValue Null = NullValue();

type::Type typeOf(const Value& value);
std::string stringify(const Value& value);

/*
  Returns a Type object representing the expression type that corresponds to
  the value type T.  (Specialized for primitives and specific array types in
  the .cpp.)
*/
template <typename T>
type::Type valueTypeToExpressionType();

/*
  Conversions between style value types and expression::Value
*/

// no-op overloads
Value toExpressionValue(const Value&);

// T = Value (just wrap in optional)
template <typename T>
std::enable_if_t<std::is_same<T, Value>::value,
optional<T>> fromExpressionValue(const Value& v)
{
    return optional<T>(v);
}

// T = member type of Value
template <typename T>
std::enable_if_t< std::is_convertible<T, Value>::value && !std::is_same<T, Value>::value,
optional<T>> fromExpressionValue(const Value& v)
{
    // This helps MSVC type deduction
    T* t;
    return v.template is<T>(t) ? v.template get<T>(t) : optional<T>();
}

// real conversions
template <typename T, typename Enable = std::enable_if_t< !std::is_convertible<T, Value>::value >>
Value toExpressionValue(const T& value);

template <typename T>
std::enable_if_t< !std::is_convertible<T, Value>::value,
optional<T>> fromExpressionValue(const Value& v);



template <class T, class Enable = void>
struct ValueConverter {
    using ExpressionType = T;
    
    static Value toExpressionValue(const T& value) {
        return Value(value);
    }
    static optional<T> fromExpressionValue(const Value& value) {
        return value.template is<T>() ? value.template get<T>() : optional<T>();
    }
};

template <>
struct ValueConverter<float> {
    using ExpressionType = double;
    static type::Type expressionType() { return type::Number; }
    static Value toExpressionValue(const float value);
    static optional<float> fromExpressionValue(const Value& value);
};

template <>
struct ValueConverter<uint64_t> {
    using ExpressionType = double;
    static type::Type expressionType() { return type::Number; }
    static Value toExpressionValue(const uint64_t& value);
    static optional<uint64_t> fromExpressionValue(const Value& value);
};
template <>
struct ValueConverter<int64_t> {
    using ExpressionType = double;
    static type::Type expressionType() { return type::Number; }
    static Value toExpressionValue(const int64_t& value);
    static optional<int64_t> fromExpressionValue(const Value& value);
};


template<>
struct ValueConverter<mbgl::Value> {
    static Value toExpressionValue(const mbgl::Value& value);
    static mbgl::Value fromExpressionValue(const Value& value);
};

template <typename T, std::size_t N>
struct ValueConverter<std::array<T, N>> {
    using ExpressionType = std::vector<Value>;
    static type::Type expressionType() {
        return type::Array(valueTypeToExpressionType<T>(), N);
    }
    static Value toExpressionValue(const std::array<T, N>& value);
    static optional<std::array<T, N>> fromExpressionValue(const Value& value);
};

template <typename T>
struct ValueConverter<std::vector<T>> {
    using ExpressionType = std::vector<Value>;
    static type::Type expressionType() {
        return type::Array(valueTypeToExpressionType<T>());
    }
    static Value toExpressionValue(const std::vector<T>& value);
    static optional<std::vector<T>> fromExpressionValue(const Value& value);
};

template <>
struct ValueConverter<Position> {
    using ExpressionType = std::vector<Value>;
    static type::Type expressionType() { return type::Array(type::Number, 3); }
    static Value toExpressionValue(const mbgl::style::Position& value);
    static optional<Position> fromExpressionValue(const Value& v);
};


template <typename T>
struct ValueConverter<T, std::enable_if_t< std::is_enum<T>::value >> {
    using ExpressionType = std::string;
    static type::Type expressionType() { return type::String; }
    static Value toExpressionValue(const T& value);
    static optional<T> fromExpressionValue(const Value& value);
};

template <typename T>
std::vector<optional<T>> fromExpressionValues(const std::vector<optional<Value>>& values) {
    std::vector<optional<T>> result;
    for (const auto& value : values) {
        result.push_back(value ? fromExpressionValue<T>(*value) : nullopt);
    }
    return result;
}

} // namespace expression
} // namespace style
} // namespace mbgl
