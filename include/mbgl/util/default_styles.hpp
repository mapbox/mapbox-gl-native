#ifndef MBGL_PLATFORM_DEFAULT_STYLES
#define MBGL_PLATFORM_DEFAULT_STYLES

#include <vector>
#include <string>

namespace mbgl {
namespace util {
namespace default_styles {

struct DefaultStyle {
    const char* url;
    const char* name;
};

extern const DefaultStyle streets;
extern const DefaultStyle outdoors;
extern const DefaultStyle light;
extern const DefaultStyle dark;
extern const DefaultStyle satellite;
extern const DefaultStyle hybrid;

const DefaultStyle orderedStyles[] = {
    streets, outdoors, light, dark, satellite, hybrid,
};
const size_t numOrderedStyles = sizeof(orderedStyles) / sizeof(DefaultStyle);

static const unsigned currentVersion = 9;

} // end namespace default_styles
} // end namespace util
} // end namespace mbgl

#endif
