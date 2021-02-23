#include <mbgl/i18n/number_format.hpp>

#if !defined(MBGL_USE_BUILTIN_ICU)
#include <unicode/numberformatter.h>
#include <unicode/uvernum.h>
#endif

#include <string>

namespace mbgl {
namespace platform {

#if !defined(MBGL_USE_BUILTIN_ICU)
std::string formatNumber(double number,
                         const std::string& localeId,
                         const std::string& currency,
                         uint8_t minFractionDigits,
                         uint8_t maxFractionDigits) {
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
                   .toString(status);
    } else {
        ustr = icu::number::NumberFormatter::with()
                   .precision(icu::number::Precision::minMaxFraction(minFractionDigits, maxFractionDigits))
                   .locale(locale)
                   .formatDouble(number, status)
                   .toString(status);
    }
    return ustr.toUTF8String(formatted);
}
#else
std::string formatNumber(double number, const std::string&, const std::string&, uint8_t, uint8_t) {
    return std::to_string(number);
}
#endif

} // namespace platform
} // namespace mbgl
