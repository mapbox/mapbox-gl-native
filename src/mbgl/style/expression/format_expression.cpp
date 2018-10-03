#include <mbgl/style/conversion_impl.hpp>
#include <mbgl/style/expression/format_expression.hpp>
#include <mbgl/style/expression/literal.hpp>
#include <mbgl/util/font_stack.hpp>
#include <mbgl/util/string.hpp>

namespace mbgl {
namespace style {
namespace expression {

FormatExpressionSection::FormatExpressionSection(std::unique_ptr<Expression> text_,
                                                 optional<std::unique_ptr<Expression>> fontScale_,
                                                 optional<std::unique_ptr<Expression>> textFont_)
    : text(std::move(text_))
{
    if (fontScale_) {
        fontScale = std::shared_ptr<Expression>(std::move(*fontScale_));
    }
    if (textFont_) {
        textFont = std::shared_ptr<Expression>(std::move(*textFont_));
    }
}

FormatExpression::FormatExpression(std::vector<FormatExpressionSection> sections_)
    : Expression(Kind::FormatExpression, type::Formatted)
    , sections(std::move(sections_))
{}

using namespace mbgl::style::conversion;

ParseResult FormatExpression::parse(const Convertible& value, ParsingContext& ctx) {
    std::size_t argsLength = arrayLength(value);
    if (argsLength < 3) {
        ctx.error("Expected at least two arguments.");
        return ParseResult();
    }
    
    if ((argsLength - 1) % 2 != 0) {
        ctx.error("Expected an even number of arguments.");
        return ParseResult();
    }
    
    std::vector<FormatExpressionSection> sections;
    for (std::size_t i = 1; i < argsLength - 1; i += 2) {
        auto textArg = arrayMember(value, i);
        ParseResult text = ctx.parse(textArg, 1, {type::Value});
        if (!text) {
            return ParseResult();
        }
        auto options = arrayMember(value, i + 1);
        if (!isObject(options)) {
            ctx.error("Format options argument must be an object.");
            return ParseResult();
        }
        
        const optional<Convertible> fontScaleOption = objectMember(options, "font-scale");
        ParseResult fontScale;
        if (fontScaleOption) {
            fontScale = ctx.parse(*fontScaleOption, 1, {type::Number});
            if (!fontScale) {
                return ParseResult();
            }
        }
        
        const optional<Convertible> textFontOption = objectMember(options, "text-font");
        ParseResult textFont;
        if (textFontOption) {
            textFont = ctx.parse(*textFontOption, 1, {type::Array(type::String)});
            if (!textFont) {
                return ParseResult();
            }
        }
        sections.emplace_back(std::move(*text), std::move(fontScale), std::move(textFont));
    }
    
    return ParseResult(std::make_unique<FormatExpression>(std::move(sections)));
}

void FormatExpression::eachChild(const std::function<void(const Expression&)>& fn) const {
    for (auto& section : sections) {
        fn(*section.text);
        if (section.fontScale) {
            fn(**section.fontScale);
        }
        if (section.textFont) {
            fn(**section.textFont);
        }
    }
}

bool FormatExpression::operator==(const Expression& e) const {
    if (e.getKind() == Kind::FormatExpression) {
        auto rhs = static_cast<const FormatExpression*>(&e);
        if (sections.size() != rhs->sections.size()) {
            return false;
        }
        for (std::size_t i = 0; i < sections.size(); i++) {
            const auto& lhsSection = sections.at(i);
            const auto& rhsSection = rhs->sections.at(i);
            if (*lhsSection.text != *rhsSection.text) {
                return false;
            }
            if ((lhsSection.fontScale && (!rhsSection.fontScale || **lhsSection.fontScale != **rhsSection.fontScale)) ||
                (!lhsSection.fontScale && rhsSection.fontScale)) {
                return false;
            }
            if ((lhsSection.textFont && (!rhsSection.textFont || **lhsSection.textFont != **rhsSection.textFont)) ||
                (!lhsSection.textFont && rhsSection.textFont)) {
                return false;
            }
        }
        return true;
    }
    return false;
}

mbgl::Value FormatExpression::serialize() const {
    std::vector<mbgl::Value> serialized{{ std::string("format") }};
    for (const auto& section : sections) {
        serialized.push_back(section.text->serialize());
        std::unordered_map<std::string, mbgl::Value> options;
        if (section.fontScale) {
            options.emplace("font-scale", (*section.fontScale)->serialize());
        }
        if (section.textFont) {
            options.emplace("text-font", (*section.textFont)->serialize());
        }
        serialized.push_back(options);
    }
    return serialized;
}

EvaluationResult FormatExpression::evaluate(const EvaluationContext& params) const {
    std::vector<FormattedSection> evaluatedSections;
    for (const auto& section : sections) {
        auto textResult = section.text->evaluate(params);
        if (!textResult) {
            return textResult.error();
        }
        
        optional<std::string> evaluatedText = toString(*textResult);
        if (!evaluatedText) {
            return EvaluationError({ "Could not coerce format expression text input to string." });
        }

        optional<double> evaluatedFontScale;
        if (section.fontScale) {
            auto fontScaleResult = (*section.fontScale)->evaluate(params);
            if (!fontScaleResult) {
                return fontScaleResult.error();
            }
            evaluatedFontScale = fontScaleResult->get<double>();
        }

        optional<FontStack> evaluatedTextFont;
        if (section.textFont) {
            auto textFontResult = (*section.textFont)->evaluate(params);
            if (!textFontResult) {
                return textFontResult.error();
            }
            auto textFontValue = ValueConverter<std::vector<std::string>>::fromExpressionValue(*textFontResult);
            if (!textFontValue) {
                return EvaluationError { "Format text-font option must evaluate to an array of strings" };
            }
            evaluatedTextFont = *textFontValue;
        }
        evaluatedSections.emplace_back(*evaluatedText, evaluatedFontScale, evaluatedTextFont);
    }
    return Formatted(evaluatedSections);
}

} // namespace expression
} // namespace style
} // namespace mbgl

