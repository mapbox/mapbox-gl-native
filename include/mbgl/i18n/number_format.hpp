#pragma once

#include <string>

namespace mbgl {
namespace platform {

std::string formatNumber(double number,
                         const std::string& localeId,
                         const std::string& currency,
                         uint8_t minFractionDigits,
                         uint8_t maxFractionDigits);

} // namespace platform
} // namespace mbgl
