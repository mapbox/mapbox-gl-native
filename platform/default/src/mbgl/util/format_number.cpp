#include <mbgl/util/platform.hpp>

#include <unicode/numberformatter.h>

namespace mbgl {
namespace platform {

std::string formatNumber(double number, const std::string& localeId, const std::string& currency,
                         uint8_t minFractionDigits, uint8_t maxFractionDigits) {

    UErrorCode status = U_ZERO_ERROR;
    icu::UnicodeString ustr;
    std::string formatted;

    icu::Locale locale = icu::Locale(localeId.c_str());
    // Print the value as currency
    if (!currency.empty()) {
        icu::UnicodeString ucurrency = icu::UnicodeString::fromUTF8(currency);
        ustr = icu::number::NumberFormatter::with()
                .unit(icu::CurrencyUnit(ucurrency.getBuffer(), status))
                .locale(locale)
                .formatDouble(number, status)
                .toString();
    } else {
        ustr = icu::number::NumberFormatter::with()
                .precision(icu::number::Precision::minMaxFraction(minFractionDigits, maxFractionDigits))
                .locale(locale)
                .formatDouble(number, status)
                .toString();
    }
    return ustr.toUTF8String(formatted);
}

} // namespace platform
} // namespace mbgl
