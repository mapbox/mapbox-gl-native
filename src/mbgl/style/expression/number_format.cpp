#include <mbgl/i18n/number_format.hpp>
#include <mbgl/style/conversion_impl.hpp>
#include <mbgl/style/expression/number_format.hpp>

namespace mbgl {
namespace style {
namespace expression {

const char localeKey[] = "locale";
const char currencyKey[] = "currency";
const char minFractionDigitsKey[] = "min-fraction-digits";
const char maxFractionDigitsKey[] = "max-fraction-digits";

NumberFormat::NumberFormat(std::unique_ptr<Expression> number_,
                           std::unique_ptr<Expression> locale_,
                           std::unique_ptr<Expression> currency_,
                           std::unique_ptr<Expression> minFractionDigits_,
                           std::unique_ptr<Expression> maxFractionDigits_)
    : Expression(Kind::NumberFormat, type::String),
      number(std::move(number_)),
      locale(std::move(locale_)),
      currency(std::move(currency_)),
      minFractionDigits(std::move(minFractionDigits_)),
      maxFractionDigits(std::move(maxFractionDigits_))
{}

NumberFormat::~NumberFormat() = default;

EvaluationResult NumberFormat::evaluate(const EvaluationContext& params) const {
    auto numberResult = number->evaluate(params);
    if (!numberResult) {
        return numberResult.error();
    }
    double evaluatedNumber = numberResult->get<double>();
    
    std::string evaluatedLocale;
    if (locale) {
        auto localeResult = locale->evaluate(params);
        if (!localeResult) {
            return localeResult.error();
        }
        evaluatedLocale = toString(*localeResult);
    }

    std::string evaluatedCurrency;
    if (currency) {
        auto currencyResult = currency->evaluate(params);
        if (!currencyResult) {
            return currencyResult.error();
        }
        evaluatedCurrency = toString(*currencyResult);
    }

    uint8_t evaluatedMinFractionDigits = 0;
    if (minFractionDigits) {
        auto minDigitsResult = minFractionDigits->evaluate(params);
        if (!minDigitsResult) {
            return minDigitsResult.error();
        }
        evaluatedMinFractionDigits = minDigitsResult->get<double>();
    }

    uint8_t evaluatedMaxFractionDigits = 3;
    if (maxFractionDigits) {
        auto maxDigitsResult = maxFractionDigits->evaluate(params);
        if (!maxDigitsResult) {
            return maxDigitsResult.error();
        }
        evaluatedMaxFractionDigits = maxDigitsResult->get<double>();
    }

    std::string output;
    output = platform::formatNumber(evaluatedNumber,
                                    evaluatedLocale,
                                    evaluatedCurrency,
                                    evaluatedMinFractionDigits,
                                    evaluatedMaxFractionDigits);
    return output;
}

void NumberFormat::eachChild(const std::function<void(const Expression&)>& visit) const {
    visit(*number);
    if (locale) visit(*locale);
    if (currency) visit(*currency);
    if (minFractionDigits) visit(*minFractionDigits);
    if (maxFractionDigits) visit(*maxFractionDigits);
}

bool NumberFormat::operator==(const Expression& e) const {
    if (e.getKind() == Kind::NumberFormat) {
        auto rhs = static_cast<const NumberFormat*>(&e);
        if ((locale && (!rhs->locale || *locale != *rhs->locale)) ||
            (!locale && rhs->locale)) {
            return false;
        }
        if ((currency && (!rhs->currency || *currency != *rhs->currency)) ||
            (!currency && rhs->currency)) {
            return false;
        }
        if ((minFractionDigits && (!rhs->minFractionDigits || *minFractionDigits != *rhs->minFractionDigits)) ||
            (!minFractionDigits && rhs->minFractionDigits)) {
            return false;
        }
        if ((maxFractionDigits && (!rhs->maxFractionDigits || *maxFractionDigits != *rhs->maxFractionDigits)) ||
            (!maxFractionDigits && rhs->maxFractionDigits)) {
            return false;
        }
        return *number == *rhs->number;
    }
    return false;
}

std::vector<optional<Value>> NumberFormat::possibleOutputs() const {
    return { nullopt };
}

using namespace mbgl::style::conversion;
ParseResult NumberFormat::parse(const Convertible& value, ParsingContext& ctx) {
    std::size_t length = arrayLength(value);

    if (length != 3) {
        ctx.error("Expected two arguments, but found " + util::toString(length) + " instead.");
        return ParseResult();
    }

    ParseResult numberResult = ctx.parse(arrayMember(value, 1), 1, {type::Number});
    if (!numberResult) {
        ctx.error("Failed to parse the number.");
        return ParseResult();
    }

    type::Type type = (*numberResult)->getType();
    if (!type.is<type::NumberType>()) {
        ctx.error("Expected argument of type number, but found " + toString(type) + " instead.");
        return ParseResult();
    }

    auto options = arrayMember(value, 2);
    if (!isObject(options)) {
        ctx.error("Number-format options argument must be an object.");
        return ParseResult();
    }

    const optional<Convertible> localeOption = objectMember(options, localeKey);
    ParseResult localeResult;
    if (localeOption) {
        localeResult = ctx.parse(*localeOption, 1, {type::String});
        if (!localeResult) {
            ctx.error("Number-format locale parsing failed.");
            return ParseResult();
        }
    }

    const optional<Convertible> currencyOption = objectMember(options, currencyKey);
    ParseResult currencyResult;
    if (currencyOption) {
        currencyResult = ctx.parse(*currencyOption, 1, {type::String});
        if (!currencyResult) {
            ctx.error("Number-format currency parsing failed.");
            return ParseResult();
        }
    }

    const optional<Convertible> minFractionDigitsOption = objectMember(options, minFractionDigitsKey);
    ParseResult minFractionDigitsResult;
    if (minFractionDigitsOption) {
        minFractionDigitsResult = ctx.parse(*minFractionDigitsOption, 1, {type::Number});
        if (!minFractionDigitsResult) {
            ctx.error("Number-format min-fraction-digits parsing failed.");
            return ParseResult();
        }
    }

    const optional<Convertible> maxFractionDigitsOption = objectMember(options, maxFractionDigitsKey);
    ParseResult maxFractionDigitsResult;
    if (maxFractionDigitsOption) {
        maxFractionDigitsResult = ctx.parse(*maxFractionDigitsOption, 1, {type::Number});
        if (!maxFractionDigitsResult) {
            ctx.error("Number-format max-fraction-digits parsing failed.");
            return ParseResult();
        }
    }

    return ParseResult(std::make_unique<NumberFormat>(std::move(*numberResult),
                                                      localeResult ? std::move(*localeResult) : nullptr,
                                                      currencyResult? std::move(*currencyResult) : nullptr,
                                                      minFractionDigitsResult ? std::move(*minFractionDigitsResult) : nullptr,
                                                      maxFractionDigitsResult ? std::move(*maxFractionDigitsResult) : nullptr));
}

mbgl::Value NumberFormat::serialize() const {
    std::vector<mbgl::Value> serialized{{ getOperator() }};
    serialized.emplace_back(number->serialize());

    std::unordered_map<std::string, mbgl::Value> options;
    if (locale) {
        options[localeKey] = locale->serialize();
    }
    if (currency) {
        options[currencyKey] = currency->serialize();
    }
    if (minFractionDigits) {
        options[minFractionDigitsKey] = minFractionDigits->serialize();
    }
    if (maxFractionDigits) {
        options[maxFractionDigitsKey] = maxFractionDigits->serialize();
    }
    serialized.emplace_back(options);

    return serialized;
}

} // namespace expression
} // namespace style
} // namespace mbgl
