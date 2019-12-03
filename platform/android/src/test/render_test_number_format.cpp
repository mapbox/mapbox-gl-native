#include <mbgl/i18n/number_format.hpp>

namespace mbgl {
namespace platform {

std::string formatNumber(double /*number*/,
                         const std::string& /*localeId */,
                         const std::string& /*currency*/,
                         uint8_t /*minFractionDigits*/,
                         uint8_t /*maxFractionDigits*/) {
    return "";
}

} // namespace platform
} // namespace mbgl
