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

// Set the name of the current thread, truncated at 15.
void setCurrentThreadName(const std::string& name);

// Makes the current thread low priority.
void makeThreadLowPriority();

// Sets priority of a current thread. Platform implementation
// must validate provided value.
void setCurrentThreadPriority(double priority);

} // namespace platform
} // namespace mbgl
