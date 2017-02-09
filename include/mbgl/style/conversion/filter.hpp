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
            return convertEqualityFilter<EqualsFilter, TypeEqualsFilter, IdentifierEqualsFilter>(value);
        } else if (*op == "!=") {
            return convertEqualityFilter<NotEqualsFilter, TypeNotEqualsFilter, IdentifierNotEqualsFilter>(value);
        } else if (*op == ">") {
            return convertBinaryFilter<GreaterThanFilter>(value);
        } else if (*op == ">=") {
            return convertBinaryFilter<GreaterThanEqualsFilter>(value);
        } else if (*op == "<") {
            return convertBinaryFilter<LessThanFilter>(value);
        } else if (*op == "<=") {
            return convertBinaryFilter<LessThanEqualsFilter>(value);
        } else if (*op == "in") {
            return convertSetFilter<InFilter, TypeInFilter, IdentifierInFilter>(value);
        } else if (*op == "!in") {
            return convertSetFilter<NotInFilter, TypeNotInFilter, IdentifierNotInFilter>(value);
        } else if (*op == "all") {
            return convertCompoundFilter<AllFilter>(value);
        } else if (*op == "any") {
            return convertCompoundFilter<AnyFilter>(value);
        } else if (*op == "none") {
            return convertCompoundFilter<NoneFilter>(value);
        } else if (*op == "has") {
            return convertUnaryFilter<HasFilter, HasIdentifierFilter>(value);
        } else if (*op == "!has") {
           return convertUnaryFilter<NotHasFilter, NotHasIdentifierFilter>(value);
        }

        return Error { "filter operator must be one of \"==\", \"!=\", \">\", \">=\", \"<\", \"<=\", \"in\", \"!in\", \"all\", \"any\", \"none\", \"has\", or \"!has\"" };
    }

private:
    Result<Value> normalizeValue(const optional<Value>& value) const {
        if (!value) {
            return Error { "filter expression value must be a boolean, number, or string" };
        } else {
            return *value;
        }
    }

    template <class V>
    Result<FeatureType> toFeatureType(const V& value) const {
        optional<std::string> type = toString(value);
        if (!type) {
            return Error { "value for $type filter must be a string" };
        } else if (*type == "Point") {
            return FeatureType::Point;
        } else if (*type == "LineString") {
            return FeatureType::LineString;
        } else if (*type == "Polygon") {
            return FeatureType::Polygon;
        } else {
            return Error { "value for $type filter must be Point, LineString, or Polygon" };
        }
    }

    template <class V>
    Result<FeatureIdentifier> toFeatureIdentifier(const V& value) const {
        optional<Value> identifier = toValue(value);
        if (!identifier) {
            return Error { "filter expression value must be a boolean, number, or string" };
        } else {
            return (*identifier).match(
                [] (uint64_t t) -> Result<FeatureIdentifier> { return t; },
                [] ( int64_t t) -> Result<FeatureIdentifier> { return t; },
                [] (  double t) -> Result<FeatureIdentifier> { return t; },
                [] (const std::string& t) -> Result<FeatureIdentifier> { return t; },
                [] (const auto&) -> Result<FeatureIdentifier> {
                    return Error { "filter expression value must be a boolean, number, or string" };
                });
        }
    }

    template <class FilterType, class IdentifierFilterType, class V>
    Result<Filter> convertUnaryFilter(const V& value) const {
        if (arrayLength(value) < 2) {
            return Error { "filter expression must have 2 elements" };
        }

        optional<std::string> key = toString(arrayMember(value, 1));
        if (!key) {
            return Error { "filter expression key must be a string" };
        }

        if (*key == "$id") {
            return IdentifierFilterType {};
        } else {
            return FilterType { *key };
        }
    }

    template <class FilterType, class TypeFilterType, class IdentifierFilterType, class V>
    Result<Filter> convertEqualityFilter(const V& value) const {
        if (arrayLength(value) < 3) {
            return Error { "filter expression must have 3 elements" };
        }

        optional<std::string> key = toString(arrayMember(value, 1));
        if (!key) {
            return Error { "filter expression key must be a string" };
        }

        if (*key == "$type") {
            Result<FeatureType> filterValue = toFeatureType(arrayMember(value, 2));
            if (!filterValue) {
                return filterValue.error();
            }

            return TypeFilterType { *filterValue };

        } else if (*key == "$id") {
            Result<FeatureIdentifier> filterValue = toFeatureIdentifier(arrayMember(value, 2));
            if (!filterValue) {
                return filterValue.error();
            }

            return IdentifierFilterType { *filterValue };

        } else {
            Result<Value> filterValue = normalizeValue(toValue(arrayMember(value, 2)));
            if (!filterValue) {
                return filterValue.error();
            }

            return FilterType { *key, *filterValue };
        }
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

        Result<Value> filterValue = normalizeValue(toValue(arrayMember(value, 2)));
        if (!filterValue) {
            return filterValue.error();
        }

        return FilterType { *key, *filterValue };
    }

    template <class FilterType, class TypeFilterType, class IdentifierFilterType, class V>
    Result<Filter> convertSetFilter(const V& value) const {
        if (arrayLength(value) < 2) {
            return Error { "filter expression must at least 2 elements" };
        }

        optional<std::string> key = toString(arrayMember(value, 1));
        if (!key) {
            return Error { "filter expression key must be a string" };
        }

        if (*key == "$type") {
            std::vector<FeatureType> values;
            for (std::size_t i = 2; i < arrayLength(value); ++i) {
                Result<FeatureType> filterValue = toFeatureType(arrayMember(value, i));
                if (!filterValue) {
                    return filterValue.error();
                }
                values.push_back(*filterValue);
            }

            return TypeFilterType { std::move(values) };

        } else if (*key == "$id") {
            std::vector<FeatureIdentifier> values;
            for (std::size_t i = 2; i < arrayLength(value); ++i) {
                Result<FeatureIdentifier> filterValue = toFeatureIdentifier(arrayMember(value, i));
                if (!filterValue) {
                    return filterValue.error();
                }
                values.push_back(*filterValue);
            }

            return IdentifierFilterType { std::move(values) };

        } else {
            std::vector<Value> values;
            for (std::size_t i = 2; i < arrayLength(value); ++i) {
                Result<Value> filterValue = normalizeValue(toValue(arrayMember(value, i)));
                if (!filterValue) {
                    return filterValue.error();
                }
                values.push_back(*filterValue);
            }

            return FilterType { *key, std::move(values) };
        }
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
