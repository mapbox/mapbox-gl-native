#pragma once

#include <unordered_map>
#include <vector>
#include <mbgl/util/optional.hpp>
#include <mbgl/util/variant.hpp>


namespace mbgl {
namespace style {
namespace expression {
namespace type {

template <class T>
std::string toString(const T& t);

struct NullType {
    constexpr NullType() = default;
    std::string getName() const { return "null"; }
    bool operator==(const NullType&) const { return true; }
};

struct NumberType {
    constexpr NumberType() = default;
    std::string getName() const { return "number"; }
    bool operator==(const NumberType&) const { return true; }
};

struct BooleanType {
    constexpr BooleanType() = default;
    std::string getName() const { return "boolean"; }
    bool operator==(const BooleanType&) const { return true; }
};

struct StringType {
    constexpr StringType() = default;
    std::string getName() const { return "string"; }
    bool operator==(const StringType&) const { return true; }
};

struct ColorType {
    constexpr ColorType() = default;
    std::string getName() const { return "color"; }
    bool operator==(const ColorType&) const { return true; }
};

struct ObjectType {
    constexpr ObjectType() = default;
    std::string getName() const { return "object"; }
    bool operator==(const ObjectType&) const { return true; }
};

struct ErrorType {
    constexpr ErrorType() = default;
    std::string getName() const { return "error"; }
    bool operator==(const ErrorType&) const { return true; }
};

struct ValueType {
    constexpr ValueType() = default;
    std::string getName() const { return "value"; }
    bool operator==(const ValueType&) const { return true; }
};

constexpr NullType Null;
constexpr NumberType Number;
constexpr StringType String;
constexpr BooleanType Boolean;
constexpr ColorType Color;
constexpr ValueType Value;
constexpr ObjectType Object;
constexpr ErrorType Error;

struct Array;

using Type = variant<
    NullType,
    NumberType,
    BooleanType,
    StringType,
    ColorType,
    ObjectType,
    ValueType,
    mapbox::util::recursive_wrapper<Array>,
    ErrorType>;

struct Array {
    Array(Type itemType_) : itemType(std::move(itemType_)) {}
    Array(Type itemType_, std::size_t N_) : itemType(std::move(itemType_)), N(N_) {}
    Array(Type itemType_, optional<std::size_t> N_) : itemType(std::move(itemType_)), N(std::move(N_)) {}
    std::string getName() const {
        if (N) {
            return "array<" + toString(itemType) + ", " + std::to_string(*N) + ">";
        } else if (itemType == Value) {
            return "array";
        } else {
            return "array<" + toString(itemType) + ">";
        }
    }

    bool operator==(const Array& rhs) const { return itemType == rhs.itemType && N == rhs.N; }
    
    Type itemType;
    optional<std::size_t> N;
};
    
template <class T>
std::string toString(const T& type) { return type.match([&] (const auto& t) { return t.getName(); }); }

} // namespace type
} // namespace expression
} // namespace style
} // namespace mbgl
