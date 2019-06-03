#pragma once

#include <string>

namespace mbgl {
namespace platform {

class Request;

// Uppercase a string, potentially using platform-specific routines.
std::string uppercase(const std::string &string);

// Lowercase a string, potentially using platform-specific routines.
std::string lowercase(const std::string &string);

// Gets the name of the current thread.
std::string getCurrentThreadName();

std::string formatNumber(double number, const std::string& localeId, const std::string& currency,
                         uint8_t minFractionDigits, uint8_t maxFractionDigits);

// Set the name of the current thread, truncated at 15.
void setCurrentThreadName(const std::string& name);

// Makes the current thread low priority.
void makeThreadLowPriority();

} // namespace platform
} // namespace mbgl
