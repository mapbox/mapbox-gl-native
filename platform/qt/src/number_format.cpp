#include <mbgl/util/platform.hpp>

#include <QLocale>
#include <QString>

namespace mbgl {
namespace platform {

std::string formatNumber(double number, const std::string& localeId, const std::string& currency,
                         uint8_t minFractionDigits, uint8_t maxFractionDigits) {

    QString formatted;
    // Qt Locale::toString() API takes only one precision argument
    (void)minFractionDigits;
    QLocale locale = QLocale(QString::fromStdString(localeId));

    if (!currency.empty()) {
        formatted = locale.toCurrencyString(number);
    } else {
        formatted = locale.toString(number, 'f', maxFractionDigits);
    }
    return formatted.toStdString();
}

} // namespace platform
} // namespace mbgl
