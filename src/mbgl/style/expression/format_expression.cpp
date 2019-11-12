#include <mbgl/style/conversion_impl.hpp>
#include <mbgl/style/expression/format_expression.hpp>
#include <mbgl/style/expression/formatted.hpp>

namespace mbgl {
namespace style {
namespace expression {

FormatExpressionSection::FormatExpressionSection(std::unique_ptr<Expression> content_) : content(std::move(content_)) {}

void FormatExpressionSection::setTextSectionOptions(optional<std::unique_ptr<Expression>> fontScale_,
                                                    optional<std::unique_ptr<Expression>> textFont_,
                                                    optional<std::unique_ptr<Expression>> textColor_) {
    if (fontScale_) {
        assert(*fontScale_);
        fontScale = std::shared_ptr<Expression>(std::move(*fontScale_));
    }

    if (textFont_) {
        assert(*textFont_);
        textFont = std::shared_ptr<Expression>(std::move(*textFont_));
    }

    if (textColor_) {
        assert(*textColor_);
        textColor = std::shared_ptr<Expression>(std::move(*textColor_));
    }
}

FormatExpression::FormatExpression(std::vector<FormatExpressionSection> sections_)
    : Expression(Kind::FormatExpression, type::Formatted)
    , sections(std::move(sections_))
{}

using namespace mbgl::style::conversion;

ParseResult FormatExpression::parse(const Convertible& value, ParsingContext& ctx) {
    std::size_t argsLength = arrayLength(value);
    if (argsLength < 2) {
        ctx.error("Expected at least one argument.");
        return ParseResult();
    }

    if (isObject(arrayMember(value, 1))) {
        ctx.error("First argument must be an image or text section.");
        return ParseResult();
    }

    std::vector<FormatExpressionSection> sections;
    bool nextTokenMayBeObject = false;
    for (std::size_t i = 1; i <= argsLength - 1; ++i) {
        auto arg = arrayMember(value, i);

        if (nextTokenMayBeObject && isObject(arg)) {
            nextTokenMayBeObject = false;

            const optional<Convertible> fontScaleOption = objectMember(arg, kFormattedSectionFontScale);
            ParseResult fontScale;
            if (fontScaleOption) {
                fontScale = ctx.parse(*fontScaleOption, 1, {type::Number});
                if (!fontScale) {
                    return ParseResult();
                }
            }

            const optional<Convertible> textFontOption = objectMember(arg, kFormattedSectionTextFont);
            ParseResult textFont;
            if (textFontOption) {
                textFont = ctx.parse(*textFontOption, 1, {type::Array(type::String)});
                if (!textFont) {
                    return ParseResult();
                }
            }
            const optional<Convertible> textColorOption = objectMember(arg, kFormattedSectionTextColor);
            ParseResult textColor;
            if (textColorOption) {
                textColor = ctx.parse(*textColorOption, 1, {type::Color});
                if (!textColor) {
                    return ParseResult();
                }
            }

            sections.back().setTextSectionOptions(std::move(fontScale), std::move(textFont), std::move(textColor));
        } else {
            ParseResult parsedArg = ctx.parse(arg, 1, {type::Value});
            if (!parsedArg) {
                ctx.error("Cannot parse formatted section.");
                return ParseResult();
            }

            nextTokenMayBeObject = true;
            sections.emplace_back(std::move(*parsedArg));
        }
    }

    return ParseResult(std::make_unique<FormatExpression>(std::move(sections)));
}

void FormatExpression::eachChild(const std::function<void(const Expression&)>& fn) const {
    for (auto& section : sections) {
        fn(*section.content);
        if (section.fontScale) {
            fn(**section.fontScale);
        }
        if (section.textFont) {
            fn(**section.textFont);
        }
        if (section.textColor) {
            fn(**section.textColor);
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
            if (*lhsSection.content != *rhsSection.content) {
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
            if ((lhsSection.textColor && (!rhsSection.textColor || **lhsSection.textColor != **rhsSection.textColor)) ||
                (!lhsSection.textColor && rhsSection.textColor)) {
                return false;
            }
        }
        return true;
    }
    return false;
}

mbgl::Value FormatExpression::serialize() const {
    std::vector<mbgl::Value> serialized{{ getOperator() }};
    for (const auto& section : sections) {
        serialized.push_back(section.content->serialize());
        std::unordered_map<std::string, mbgl::Value> options;
        if (section.fontScale) {
            options.emplace(kFormattedSectionFontScale, (*section.fontScale)->serialize());
        }
        if (section.textFont) {
            options.emplace(kFormattedSectionTextFont, (*section.textFont)->serialize());
        }
        if (section.textColor) {
            options.emplace(kFormattedSectionTextColor, (*section.textColor)->serialize());
        }
        serialized.emplace_back(options);
    }
    return serialized;
}

EvaluationResult FormatExpression::evaluate(const EvaluationContext& params) const {
    std::vector<FormattedSection> evaluatedSections;
    for (const auto& section : sections) {
        auto contentResult = section.content->evaluate(params);
        if (!contentResult) {
            return contentResult.error();
        }

        optional<std::string> evaluatedText;
        if (typeOf(*contentResult) == type::Image) {
            const auto& image = contentResult->get<Image>();
            // Omit sections with empty image ids.
            if (!image.id().empty()) {
                evaluatedSections.emplace_back(image);
            }
            // Continue evaluation of a next section, as the image section does not have section options.
            continue;
        } else {
            evaluatedText = toString(*contentResult);
            if (!evaluatedText) {
                return EvaluationError({"Could not coerce format expression text input to string."});
            }
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

        optional<Color> evaluatedTextColor;
        if (section.textColor) {
            auto textColorResult = (*section.textColor)->evaluate(params);
            if (!textColorResult) {
                return textColorResult.error();
            }

            evaluatedTextColor = fromExpressionValue<Color>(*textColorResult);
            if (!evaluatedTextColor) {
                return EvaluationError { "Format text-color option must evaluate to Color" };
            }
        }
        evaluatedSections.emplace_back(*evaluatedText, evaluatedFontScale, evaluatedTextFont, evaluatedTextColor);
    }
    return Formatted(evaluatedSections);
}

} // namespace expression
} // namespace style
} // namespace mbgl

