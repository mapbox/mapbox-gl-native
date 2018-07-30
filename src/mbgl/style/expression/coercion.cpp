#include <mbgl/style/expression/coercion.hpp>
#include <mbgl/style/expression/check_subtype.hpp>
#include <mbgl/style/expression/util.hpp>
#include <mbgl/util/string.hpp>

namespace mbgl {
namespace style {
namespace expression {

EvaluationResult toNumber(const Value& v) {
    optional<double> result = v.match(
        [](const double f) -> optional<double> { return f; },
        [](const std::string& s) -> optional<double> {
            try {
                return util::stof(s);
            } catch (const std::exception&) {
                return optional<double>();
            }
        },
        [](const auto&) { return optional<double>(); }
    );
    if (!result) {
        return EvaluationError {
            "Could not convert " + stringify(v) + " to number."
        };
    }
    return *result;
}

EvaluationResult toColor(const Value& colorValue) {
    return colorValue.match(
        [&](const std::string& colorString) -> EvaluationResult {
            const optional<Color> result = Color::parse(colorString);
            if (result) {
                return *result;
            } else {
                return EvaluationError{
                    "Could not parse color from value '" + colorString + "'"
                };
            }
        },
        [&](const std::vector<Value>& components) -> EvaluationResult {
            std::size_t len = components.size();
            bool isNumeric = std::all_of(components.begin(), components.end(), [](const Value& item) {
                return item.template is<double>();
            });
            if ((len == 3 || len == 4) && isNumeric) {
                Result<Color> c = {rgba(
                    components[0].template get<double>(),
                    components[1].template get<double>(),
                    components[2].template get<double>(),
                    len == 4 ? components[3].template get<double>() : 1.0
                )};
                if (!c) return c.error();
                return *c;
            } else {
                return EvaluationError{
                    "Invalid rbga value " + stringify(colorValue) + ": expected an array containing either three or four numeric values."
                };
            }
        },
        [&](const auto&) -> EvaluationResult {
            return EvaluationError{
                "Could not parse color from value '" + stringify(colorValue) + "'"
            };
        }
    );
}

Coercion::Coercion(type::Type type_, std::vector<std::unique_ptr<Expression>> inputs_) :
    Expression(Kind::Coercion, std::move(type_)),
    inputs(std::move(inputs_))
{
    assert(!inputs.empty());
    type::Type t = getType();
    if (t.is<type::NumberType>()) {
        coerceSingleValue = toNumber;
    } else if (t.is<type::ColorType>()) {
        coerceSingleValue = toColor;
    } else {
        assert(false);
    }
}

std::string Coercion::getOperator() const {
    return getType().match(
      [](const type::NumberType&) { return "to-number"; },
      [](const type::ColorType&) { return "to-color"; },
      [](const auto&) { assert(false); return ""; });
}

using namespace mbgl::style::conversion;
ParseResult Coercion::parse(const Convertible& value, ParsingContext& ctx) {
    static std::unordered_map<std::string, type::Type> types {
        {"to-number", type::Number},
        {"to-color", type::Color}
    };

    std::size_t length = arrayLength(value);

    if (length < 2) {
        ctx.error("Expected at least one argument.");
        return ParseResult();
    }

    auto it = types.find(*toString(arrayMember(value, 0)));
    assert(it != types.end());
    
    std::vector<std::unique_ptr<Expression>> parsed;
    parsed.reserve(length - 1);
    for (std::size_t i = 1; i < length; i++) {
        ParseResult input = ctx.parse(arrayMember(value, i), i, {type::Value});
        if (!input) return ParseResult();
        parsed.push_back(std::move(*input));
    }

    return ParseResult(std::make_unique<Coercion>(it->second, std::move(parsed)));
}

EvaluationResult Coercion::evaluate(const EvaluationContext& params) const {
    for (std::size_t i = 0; i < inputs.size(); i++) {
        EvaluationResult value = inputs[i]->evaluate(params);
        if (!value) return value;
        EvaluationResult coerced = coerceSingleValue(*value);
        if (coerced || i == inputs.size() - 1) {
            return coerced;
        }
    }

    assert(false);
    return EvaluationError { "Unreachable" };
};

void Coercion::eachChild(const std::function<void(const Expression&)>& visit) const {
    for(const std::unique_ptr<Expression>& input : inputs) {
        visit(*input);
    }
};

bool Coercion::operator==(const Expression& e) const {
    if (e.getKind() == Kind::Coercion) {
        auto rhs = static_cast<const Coercion*>(&e);
        return getType() == rhs->getType() && Expression::childrenEqual(inputs, rhs->inputs);
    }
    return false;
}

std::vector<optional<Value>> Coercion::possibleOutputs() const {
    std::vector<optional<Value>> result;
    for (const auto& input : inputs) {
        for (auto& output : input->possibleOutputs()) {
            result.push_back(std::move(output));
        }
    }
    return result;
}

} // namespace expression
} // namespace style
} // namespace mbgl



