#include <mbgl/style/expression/literal.hpp>
#include <mbgl/util/string.hpp>

namespace mbgl {
namespace style {
namespace expression {

template <typename T>
optional<Value> checkNumber(T n) {
    if (n > std::numeric_limits<double>::max()) {
        return {std::numeric_limits<double>::infinity()};
    } else {
        return {static_cast<double>(n)};
    }
}

using namespace mbgl::style::conversion;
optional<Value> parseValue(const Convertible& value, ParsingContext& ctx) {
    if (isUndefined(value)) return {Null};
    if (isObject(value)) {
        std::unordered_map<std::string, Value> result;
        bool error = false;
        eachMember(value, [&] (const std::string& k, const mbgl::style::conversion::Convertible& v) -> optional<conversion::Error> {
            if (!error) {
                optional<Value> memberValue = parseValue(v, ctx);
                if (memberValue) {
                    result.emplace(k, *memberValue);
                } else {
                    error = true;
                }
            }
            return {};
        });
        return error ? optional<Value>() : optional<Value>(result);
    }
    
    if (isArray(value)) {
        std::vector<Value> result;
        const auto length = arrayLength(value);
        for(std::size_t i = 0; i < length; i++) {
            optional<Value> item = parseValue(arrayMember(value, i), ctx);
            if (item) {
                result.emplace_back(*item);
            } else {
                return optional<Value>();
            }
        }
        return optional<Value>(result);
    }
    
    optional<mbgl::Value> v = toValue(value);
    // since value represents a JSON value, if it's not undefined, object, or
    // array, it must be convertible to mbgl::Value
    assert(v);
    
    return v->match(
        [&](uint64_t n) { return checkNumber(n); },
        [&](int64_t n) { return checkNumber(n); },
        [&](double n) { return checkNumber(n); },
        [&](const auto&) {
            return optional<Value>(toExpressionValue(*v));
        }
    );
}

ParseResult Literal::parse(const Convertible& value, ParsingContext& ctx) {
    if (isObject(value)) {
        ctx.error(R"(Bare objects invalid. Use ["literal", {...}] instead.)");
        return ParseResult();
    } else if (isArray(value)) {
        // object or array value, quoted with ["literal", value]
        if (arrayLength(value) != 2) {
            ctx.error("'literal' expression requires exactly one argument, but found " + util::toString(arrayLength(value) - 1) + " instead.");
            return ParseResult();
        }
        const optional<Value> parsedValue = parseValue(arrayMember(value, 1), ctx);
        if (!parsedValue) {
            return ParseResult();
        }
        
        // special case: infer the item type if possible for zero-length arrays
        if (
            ctx.getExpected() &&
            ctx.getExpected()->template is<type::Array>() &&
            parsedValue->template is<std::vector<Value>>()
        ) {
            auto type = typeOf(*parsedValue).template get<type::Array>();
            auto expected = ctx.getExpected()->template get<type::Array>();
            if (
                type.N && (*type.N == 0) &&
                (!expected.N || (*expected.N == 0))
            ) {
                return ParseResult(std::make_unique<Literal>(expected, parsedValue->template get<std::vector<Value>>()));
            }
        }
        
        return ParseResult(std::make_unique<Literal>(*parsedValue));
    } else {
        // bare primitive value (string, number, boolean, null)
        const optional<Value> parsedValue = parseValue(value, ctx);
        return ParseResult(std::make_unique<Literal>(*parsedValue));
    }
}

mbgl::Value Literal::serialize() const {
    if (getType().is<type::Array>() || getType().is<type::ObjectType>()) {
        return std::vector<mbgl::Value>{{ getOperator(), *fromExpressionValue<mbgl::Value>(value) }};
    } else {
        return *fromExpressionValue<mbgl::Value>(value);
    }
}

} // namespace expression
} // namespace style
} // namespace mbgl

