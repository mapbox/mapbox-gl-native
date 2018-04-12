#include <mbgl/style/conversion/filter.hpp>
#include <mbgl/util/geometry.hpp>
#include <mbgl/style/expression/expression.hpp>
#include <mbgl/style/expression/type.hpp>
#include <mbgl/style/expression/parsing_context.hpp>

namespace mbgl {
namespace style {
namespace conversion {
    
using GeometryValue = mapbox::geometry::value;

// This is a port from https://github.com/mapbox/mapbox-gl-js/blob/master/src/style-spec/feature_filter/index.js
static bool isExpressionFilter(const Convertible& filter) {
    if (!isArray(filter) || arrayLength(filter) == 0) {
        return false;
    }
    
    optional<std::string> op = toString(arrayMember(filter, 0));
    
    if (!op) {
        return false;

    } else if (*op == "has") {
        if (arrayLength(filter) < 2) return false;
        optional<std::string> operand = toString(arrayMember(filter, 1));
        return operand && *operand != "$id" && *operand != "$type";
        
    } else if (*op == "in" || *op == "!in" || *op == "!has" || *op == "none") {
        return false;
        
    } else if (*op == "==" || *op == "!=" || *op == ">" || *op == ">=" || *op == "<" || *op == "<=") {
        return arrayLength(filter) == 3 && (isArray(arrayMember(filter, 1)) || isArray(arrayMember(filter, 2)));
        
    } else if (*op == "any" || *op == "all") {
        for (std::size_t i = 1; i < arrayLength(filter); i++) {
            Convertible f = arrayMember(filter, i);
            if (!isExpressionFilter(f) && !toBool(f)) {
                return false;
            }
        }
        return true;
        
    } else {
        return true;
    }
}
    
static optional<GeometryValue> normalizeValue(const optional<GeometryValue>& value, Error& error) {
    if (!value) {
        error = { "filter expression value must be a boolean, number, or string" };
        return {};
    } else {
        return *value;
    }
}

static optional<FeatureType> toFeatureType(const Convertible& value, Error& error) {
    optional<std::string> type = toString(value);
    if (!type) {
        error = { "value for $type filter must be a string" };
        return {};
    } else if (*type == "Point") {
        return FeatureType::Point;
    } else if (*type == "LineString") {
        return FeatureType::LineString;
    } else if (*type == "Polygon") {
        return FeatureType::Polygon;
    } else {
        error = { "value for $type filter must be Point, LineString, or Polygon" };
        return {};
    }
}

static optional<FeatureIdentifier> toFeatureIdentifier(const Convertible& value, Error& error) {
    optional<GeometryValue> identifier = toValue(value);
    if (!identifier) {
        error = { "filter expression value must be a boolean, number, or string" };
        return {};
    } else {
        return (*identifier).match(
            [] (uint64_t t) -> optional<FeatureIdentifier> { return { t }; },
            [] ( int64_t t) -> optional<FeatureIdentifier> { return { t }; },
            [] (  double t) -> optional<FeatureIdentifier> { return { t }; },
            [] (const std::string& t) -> optional<FeatureIdentifier> { return { t }; },
            [&] (const auto&) -> optional<FeatureIdentifier> {
                error = { "filter expression value must be a boolean, number, or string" };
                return {};
            });
    }
}

template <class FilterType, class IdentifierFilterType>
optional<Filter> convertUnaryFilter(const Convertible& value, Error& error) {
    if (arrayLength(value) < 2) {
        error = { "filter expression must have 2 elements" };
        return {};
    }

    optional<std::string> key = toString(arrayMember(value, 1));
    if (!key) {
        error = { "filter expression key must be a string" };
        return {};
    }

    if (*key == "$id") {
        return { IdentifierFilterType {} };
    } else {
        return { FilterType { *key } };
    }
}

template <class FilterType, class TypeFilterType, class IdentifierFilterType>
optional<Filter> convertEqualityFilter(const Convertible& value, Error& error) {
    if (arrayLength(value) < 3) {
        error = { "filter expression must have 3 elements" };
        return {};
    }

    optional<std::string> key = toString(arrayMember(value, 1));
    if (!key) {
        error = { "filter expression key must be a string" };
        return {};
    }

    if (*key == "$type") {
        optional<FeatureType> filterValue = toFeatureType(arrayMember(value, 2), error);
        if (!filterValue) {
            return {};
        }

        return { TypeFilterType { *filterValue } };

    } else if (*key == "$id") {
        optional<FeatureIdentifier> filterValue = toFeatureIdentifier(arrayMember(value, 2), error);
        if (!filterValue) {
            return {};
        }

        return { IdentifierFilterType { *filterValue } };

    } else {
        optional<GeometryValue> filterValue = normalizeValue(toValue(arrayMember(value, 2)), error);
        if (!filterValue) {
            return {};
        }

        return { FilterType { *key, *filterValue } };
    }
}

template <class FilterType>
optional<Filter> convertBinaryFilter(const Convertible& value, Error& error) {
    if (arrayLength(value) < 3) {
        error = { "filter expression must have 3 elements" };
        return {};
    }

    optional<std::string> key = toString(arrayMember(value, 1));
    if (!key) {
        error = { "filter expression key must be a string" };
        return {};
    }

    optional<GeometryValue> filterValue = normalizeValue(toValue(arrayMember(value, 2)), error);
    if (!filterValue) {
        return {};
    }

    return { FilterType { *key, *filterValue } };
}

template <class FilterType, class TypeFilterType, class IdentifierFilterType>
optional<Filter> convertSetFilter(const Convertible& value, Error& error) {
    if (arrayLength(value) < 2) {
        error = { "filter expression must at least 2 elements" };
        return {};
    }

    optional<std::string> key = toString(arrayMember(value, 1));
    if (!key) {
        error = { "filter expression key must be a string" };
        return {};
    }

    if (*key == "$type") {
        std::vector<FeatureType> values;
        for (std::size_t i = 2; i < arrayLength(value); ++i) {
            optional<FeatureType> filterValue = toFeatureType(arrayMember(value, i), error);
            if (!filterValue) {
                return {};
            }
            values.push_back(*filterValue);
        }

        return { TypeFilterType { std::move(values) } };

    } else if (*key == "$id") {
        std::vector<FeatureIdentifier> values;
        for (std::size_t i = 2; i < arrayLength(value); ++i) {
            optional<FeatureIdentifier> filterValue = toFeatureIdentifier(arrayMember(value, i), error);
            if (!filterValue) {
                return {};
            }
            values.push_back(*filterValue);
        }

        return { IdentifierFilterType { std::move(values) } };

    } else {
        std::vector<GeometryValue> values;
        for (std::size_t i = 2; i < arrayLength(value); ++i) {
            optional<GeometryValue> filterValue = normalizeValue(toValue(arrayMember(value, i)), error);
            if (!filterValue) {
                return {};
            }
            values.push_back(*filterValue);
        }

        return { FilterType { *key, std::move(values) } };
    }
}

template <class FilterType>
optional<Filter> convertCompoundFilter(const Convertible& value, Error& error) {
    std::vector<Filter> filters;
    for (std::size_t i = 1; i < arrayLength(value); ++i) {
        optional<Filter> element = convert<Filter>(arrayMember(value, i), error);
        if (!element) {
            return {};
        }
        filters.push_back(*element);
    }

    return { FilterType { std::move(filters) } };
}
    
optional<Filter> convertExpressionFilter(const Convertible& value, Error& error) {
    expression::ParsingContext ctx(expression::type::Boolean);
    expression::ParseResult expression = ctx.parseExpression(value);
    if (!expression) {
        error = { ctx.getCombinedErrors() };
        return {};
    }

    return { ExpressionFilter { std::move(*expression) } };
}

optional<Filter> Converter<Filter>::operator()(const Convertible& value, Error& error) const {
    if (isExpressionFilter(value)) {
        return convertExpressionFilter(value, error);
    }
    
    if (!isArray(value)) {
        error = { "filter expression must be an array" };
        return {};
    }

    if (arrayLength(value) < 1) {
        error = { "filter expression must have at least 1 element" };
        return {};
    }

    optional<std::string> op = toString(arrayMember(value, 0));
    if (!op) {
        error = { "filter operator must be a string" };
        return {};
    }

    if (*op == "==") {
        return convertEqualityFilter<EqualsFilter, TypeEqualsFilter, IdentifierEqualsFilter>(value, error);
    } else if (*op == "!=") {
        return convertEqualityFilter<NotEqualsFilter, TypeNotEqualsFilter, IdentifierNotEqualsFilter>(value, error);
    } else if (*op == ">") {
        return convertBinaryFilter<GreaterThanFilter>(value, error);
    } else if (*op == ">=") {
        return convertBinaryFilter<GreaterThanEqualsFilter>(value, error);
    } else if (*op == "<") {
        return convertBinaryFilter<LessThanFilter>(value, error);
    } else if (*op == "<=") {
        return convertBinaryFilter<LessThanEqualsFilter>(value, error);
    } else if (*op == "in") {
        return convertSetFilter<InFilter, TypeInFilter, IdentifierInFilter>(value, error);
    } else if (*op == "!in") {
        return convertSetFilter<NotInFilter, TypeNotInFilter, IdentifierNotInFilter>(value, error);
    } else if (*op == "all") {
        return convertCompoundFilter<AllFilter>(value, error);
    } else if (*op == "any") {
        return convertCompoundFilter<AnyFilter>(value, error);
    } else if (*op == "none") {
        return convertCompoundFilter<NoneFilter>(value, error);
    } else if (*op == "has") {
        return convertUnaryFilter<HasFilter, HasIdentifierFilter>(value, error);
    } else if (*op == "!has") {
       return convertUnaryFilter<NotHasFilter, NotHasIdentifierFilter>(value, error);
    }

    error = { R"(filter operator must be one of "==", "!=", ">", ">=", "<", "<=", "in", "!in", "all", "any", "none", "has", or "!has")" };
    return {};
}

} // namespace conversion
} // namespace style
} // namespace mbgl
