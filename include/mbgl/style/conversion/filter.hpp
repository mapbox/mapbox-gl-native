#pragma once

#include <mbgl/style/filter.hpp>
#include <mbgl/style/conversion.hpp>
#include <mbgl/util/geometry.hpp>

namespace mbgl {
namespace style {
namespace conversion {

template <>
struct Converter<Filter> {
public:
    template <class V>
    Result<Filter> operator()(const V& value) const {
        if (!isArray(value)) {
            return Error { "filter expression must be an array" };
        }

        if (arrayLength(value) < 1) {
            return Error { "filter expression must have at least 1 element" };
        }

        optional<std::string> op = toString(arrayMember(value, 0));
        if (!op) {
            return Error { "filter operator must be a string" };
        }

        if (*op == "==") {
            return convertBinaryFilter<EqualsFilter>(value);
        } else if (*op == "!=") {
            return convertBinaryFilter<NotEqualsFilter>(value);
        } else if (*op == ">") {
            return convertBinaryFilter<GreaterThanFilter>(value);
        } else if (*op == ">=") {
            return convertBinaryFilter<GreaterThanEqualsFilter>(value);
        } else if (*op == "<") {
            return convertBinaryFilter<LessThanFilter>(value);
        } else if (*op == "<=") {
            return convertBinaryFilter<LessThanEqualsFilter>(value);
        } else if (*op == "in") {
            return convertSetFilter<InFilter>(value);
        } else if (*op == "!in") {
            return convertSetFilter<NotInFilter>(value);
        } else if (*op == "all") {
            return convertCompoundFilter<AllFilter>(value);
        } else if (*op == "any") {
            return convertCompoundFilter<AnyFilter>(value);
        } else if (*op == "none") {
            return convertCompoundFilter<NoneFilter>(value);
        } else if (*op == "has") {
            return convertUnaryFilter<HasFilter>(value);
        } else if (*op == "!has") {
           return convertUnaryFilter<NotHasFilter>(value);
        }

        return Error { "filter operator must be one of \"==\", \"!=\", \">\", \">=\", \"<\", \"<=\", \"in\", \"!in\", \"all\", \"any\", \"none\", \"has\", or \"!has\"" };
    }

private:
    Result<Value> normalizeValue(const std::string& key, const optional<Value>& value) const {
        if (!value) {
            return Error { "filter expression value must be a boolean, number, or string" };
        } else if (key != "$type") {
            return *value;
        } else if (*value == std::string("Point")) {
            return Value(uint64_t(FeatureType::Point));
        } else if (*value == std::string("LineString")) {
            return Value(uint64_t(FeatureType::LineString));
        } else if (*value == std::string("Polygon")) {
            return Value(uint64_t(FeatureType::Polygon));
        } else {
            return Error { "value for $type filter must be Point, LineString, or Polygon" };
        }
    }

    template <class FilterType, class V>
    Result<Filter> convertUnaryFilter(const V& value) const {
        if (arrayLength(value) < 2) {
            return Error { "filter expression must have 2 elements" };
        }

        optional<std::string> key = toString(arrayMember(value, 1));
        if (!key) {
            return Error { "filter expression key must be a string" };
        }

        return FilterType { *key };
    }

    template <class FilterType, class V>
    Result<Filter> convertBinaryFilter(const V& value) const {
        if (arrayLength(value) < 3) {
            return Error { "filter expression must have 3 elements" };
        }

        optional<std::string> key = toString(arrayMember(value, 1));
        if (!key) {
            return Error { "filter expression key must be a string" };
        }

        Result<Value> filterValue = normalizeValue(*key, toValue(arrayMember(value, 2)));
        if (!filterValue) {
            return filterValue.error();
        }

        return FilterType { *key, *filterValue };
    }

    template <class FilterType, class V>
    Result<Filter> convertSetFilter(const V& value) const {
        if (arrayLength(value) < 2) {
            return Error { "filter expression must at least 2 elements" };
        }

        optional<std::string> key = toString(arrayMember(value, 1));
        if (!key) {
            return Error { "filter expression key must be a string" };
        }

        std::vector<Value> values;
        for (std::size_t i = 2; i < arrayLength(value); ++i) {
            Result<Value> filterValue = normalizeValue(*key, toValue(arrayMember(value, i)));
            if (!filterValue) {
                return filterValue.error();
            }
            values.push_back(*filterValue);
        }

        return FilterType { *key, std::move(values) };
    }

    template <class FilterType, class V>
    Result<Filter> convertCompoundFilter(const V& value) const {
        std::vector<Filter> filters;
        for (std::size_t i = 1; i < arrayLength(value); ++i) {
            Result<Filter> element = operator()(arrayMember(value, i));
            if (!element) {
                return element.error();
            }
            filters.push_back(*element);
        }

        return FilterType { std::move(filters) };
    }
};

} // namespace conversion
} // namespace style
} // namespace mbgl
