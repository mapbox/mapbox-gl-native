#ifndef MBGL_PLATFORM_DEFAULT_STYLES
#define MBGL_PLATFORM_DEFAULT_STYLES

#include <vector>
#include <string>

namespace mbgl {
namespace util {

// A list of default styles, with the first string being the URL
// and the second being the user-visible name.
extern const std::vector<std::pair<std::string, std::string>> defaultStyles;

} // end namespace util
} // end namespace mbgl

#endif
