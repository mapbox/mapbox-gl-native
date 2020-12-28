#pragma once

#include <chrono>

namespace mbgl {
namespace platform {

// Returns the current time. Abstracted because some platforms
// will not allow direct access to the current time via syscall.
std::chrono::time_point<std::chrono::system_clock, std::chrono::seconds> now();

} // namespace platform
} // namespace mbgl
