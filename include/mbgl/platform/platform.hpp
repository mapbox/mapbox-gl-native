#ifndef MBGL_PLATFORM_PLATFORM
#define MBGL_PLATFORM_PLATFORM

#include <memory>
#include <string>

namespace mbgl {
namespace platform {

class Request;

// Uppercase a string, potentially using platform-specific routines.
std::string uppercase(const std::string &string);

// Lowercase a string, potentially using platform-specific routines.
std::string lowercase(const std::string &string);

// Returns the path to the root folder of the application.
const std::string &applicationRoot();

// Returns the path to the asset location.
const std::string &assetRoot();

// Makes the current thread low priority.
void makeThreadLowPriority();

// Shows an alpha image with the specified dimensions in a named window.
void showDebugImage(std::string name, const char *data, size_t width, size_t height);

// Shows an alpha image with the specified dimensions in a named window.
void showColorDebugImage(std::string name, const char *data, size_t logical_width, size_t logical_height, size_t width, size_t height);
}
}

#endif
