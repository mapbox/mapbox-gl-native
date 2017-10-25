#pragma once

#include <memory>
#include <mbgl/style/expression/parsing_context.hpp>
#include <mbgl/style/expression/type.hpp>
#include <mbgl/style/conversion.hpp>

namespace mbgl {
namespace style {
namespace conversion {

using namespace mbgl::style::expression;

template<> struct Converter<std::unique_ptr<Expression>> {
    optional<std::unique_ptr<Expression>> operator()(const mbgl::style::conversion::Convertible& value, Error& error, type::Type expected) const {
        std::vector<ParsingError> errors;
        ParseResult parsed = ParsingContext(errors, expected).parse(value);
        if (parsed) {
            return std::move(*parsed);
        }
        std::string combinedError;
        for (const ParsingError& parsingError : errors) {
            if (combinedError.size() > 0) {
                combinedError += "\n";
            }
            if (parsingError.key.size() > 0) {
                combinedError += parsingError.key + ": ";
            }
            combinedError += parsingError.message;
        }
        error = { combinedError };
        return {};
    };
};

} // namespace conversion
} // namespace style
} // namespace mbgl
