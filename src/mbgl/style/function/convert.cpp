#include <mbgl/style/function/convert.hpp>

namespace mbgl {
namespace style {
namespace expression {

std::unique_ptr<Expression> Convert::fromIdentityFunction(const std::string& property, type::Type type) {
    return type.match(
        [&] (const type::StringType&) {
            return makeGet(type::String, property);
        },
        [&] (const type::NumberType&) {
            return makeGet(type::Number, property);
        },
        [&] (const type::BooleanType&) {
            return makeGet(type::Boolean, property);
        },
        [&] (const type::ColorType&) {
            std::vector<std::unique_ptr<Expression>> args;
            args.push_back(makeGet(type::String, property));
            return std::make_unique<Coercion>(type::Color, std::move(args));
        },
        [&] (const type::Array& arr) {
            std::vector<std::unique_ptr<Expression>> getArgs;
            getArgs.push_back(makeLiteral(property));
            ParsingContext ctx;
            ParseResult get = createCompoundExpression("get", std::move(getArgs), ctx);
            assert(get);
            assert(ctx.getErrors().size() == 0);
            return std::make_unique<ArrayAssertion>(arr, std::move(*get));
        },
        [&] (const auto&) -> std::unique_ptr<Expression> {
            return makeLiteral(Null);
        }
    );
}

} // namespace expression
} // namespace style
} // namespace mbgl
