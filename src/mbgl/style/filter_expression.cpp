#include <mbgl/style/filter_expression.hpp>
#include <mbgl/map/geometry_tile.hpp>
#include <mbgl/platform/log.hpp>

namespace mbgl {

Value parseFeatureType(const Value& value) {
    if (value == std::string("Point")) {
        return Value(uint64_t(FeatureType::Point));
    } else if (value == std::string("LineString")) {
        return Value(uint64_t(FeatureType::LineString));
    } else if (value == std::string("Polygon")) {
        return Value(uint64_t(FeatureType::Polygon));
    } else {
        Log::Warning(Event::ParseStyle, "value for $type filter must be Point, LineString, or Polygon");
        return Value(uint64_t(FeatureType::Unknown));
    }
}

template <class Expression>
FilterExpression parseBinaryFilter(const rapidjson::Value& value) {
    FilterExpression empty;

    if (value.Size() < 3) {
        Log::Warning(Event::ParseStyle, "filter expression must have 3 elements");
        return empty;
    }

    if (!value[1u].IsString()) {
        Log::Warning(Event::ParseStyle, "filter expression key must be a string");
        return empty;
    }

    Expression expression;
    expression.key = { value[1u].GetString(), value[1u].GetStringLength() };
    expression.value = parseValue(value[2u]);

    if (expression.key == "$type") {
        expression.value = parseFeatureType(expression.value);
    }

    return expression;
}

template <class Expression>
FilterExpression parseSetFilter(const rapidjson::Value& value) {
    FilterExpression empty;

    if (value.Size() < 2) {
        Log::Warning(Event::ParseStyle, "filter expression must at least 2 elements");
        return empty;
    }

    if (!value[1u].IsString()) {
        Log::Warning(Event::ParseStyle, "filter expression key must be a string");
        return empty;
    }

    Expression expression;
    expression.key = { value[1u].GetString(), value[1u].GetStringLength() };
    for (rapidjson::SizeType i = 2; i < value.Size(); ++i) {
        Value parsedValue = parseValue(value[i]);
        if (expression.key == "$type") {
            parsedValue = parseFeatureType(parsedValue);
        }
        expression.values.push_back(parsedValue);
    }
    return expression;
}

template <class Expression>
FilterExpression parseCompoundFilter(const rapidjson::Value& value) {
    Expression expression;
    for (rapidjson::SizeType i = 1; i < value.Size(); ++i) {
        expression.expressions.push_back(parseFilterExpression(value[i]));
    }
    return expression;
}

FilterExpression parseFilterExpression(const rapidjson::Value& value) {
    FilterExpression empty;

    if (!value.IsArray()) {
        Log::Warning(Event::ParseStyle, "filter expression must be an array");
        return empty;
    }

    if (value.Size() < 1) {
        Log::Warning(Event::ParseStyle, "filter expression must have at least 1 element");
        return empty;
    }

    if (!value[0u].IsString()) {
        Log::Warning(Event::ParseStyle, "filter operator must be a string");
        return empty;
    }

    std::string op = { value[0u].GetString(), value[0u].GetStringLength() };

    if (op == "==") {
        return parseBinaryFilter<EqualsExpression>(value);
    } else if (op == "!=") {
        return parseBinaryFilter<NotEqualsExpression>(value);
    } else if (op == ">") {
        return parseBinaryFilter<GreaterThanExpression>(value);
    } else if (op == ">=") {
        return parseBinaryFilter<GreaterThanEqualsExpression>(value);
    } else if (op == "<") {
        return parseBinaryFilter<LessThanExpression>(value);
    } else if (op == "<=") {
        return parseBinaryFilter<LessThanEqualsExpression>(value);
    } else if (op == "in") {
        return parseSetFilter<InExpression>(value);
    } else if (op == "!in") {
        return parseSetFilter<NotInExpression>(value);
    } else if (op == "all") {
        return parseCompoundFilter<AllExpression>(value);
    } else if (op == "any") {
        return parseCompoundFilter<AnyExpression>(value);
    } else if (op == "none") {
        return parseCompoundFilter<NoneExpression>(value);
    } else {
        Log::Warning(Event::ParseStyle, "filter operator must be one of \"==\", \"!=\", \">\", \">=\", \"<\", \"<=\", \"in\", \"!in\", \"all\", \"any\", \"none\"");
        return empty;
    }
}

}
