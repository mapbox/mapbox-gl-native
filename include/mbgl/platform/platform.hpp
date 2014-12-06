#ifndef MBGL_PLATFORM_PLATFORM
#define MBGL_PLATFORM_PLATFORM

#include <mbgl/util/uv.hpp>

#include <memory>
#include <string>

namespace mbgl {
namespace platform {

class Request;

// Uppercase a string, potentially using platform-specific routines.
std::string uppercase(const std::string &string);

// Lowercase a string, potentially using platform-specific routines.
std::string lowercase(const std::string &string);

// Returns the path to the default cache database on this system.
std::string defaultCacheDatabase();

// Returns the path to the default shader cache on this system.
std::string defaultShaderCache();

// Shows an alpha image with the specified dimensions in a named window.
void show_debug_image(std::string name, const char *data, size_t width, size_t height);

// Shows an alpha image with the specified dimensions in a named window.
void show_color_debug_image(std::string name, const char *data, size_t logical_width, size_t logical_height, size_t width, size_t height);
}
}

#endif
