#include <mbgl/util/platform.hpp>

#if 0
#include <unicode/numberformatter.h>
#else
#include <mbgl/util/string.hpp>
#endif

namespace mbgl {
namespace platform {

std::string formatNumber(double number, const std::string& localeId, const std::string& currency,
                         uint8_t minFractionDigits, uint8_t maxFractionDigits) {

#if 0
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
#else
    (void)localeId;
    (void)currency;
    (void)minFractionDigits;
    (void)maxFractionDigits;
    return mbgl::util::toString(number);
#endif
}

} // namespace platform
} // namespace mbgl
