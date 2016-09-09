#pragma once

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
extern const DefaultStyle satelliteStreets;

const DefaultStyle orderedStyles[] = {
    streets, outdoors, light, dark, satellite, satelliteStreets,
};
const size_t numOrderedStyles = sizeof(orderedStyles) / sizeof(DefaultStyle);

static const unsigned currentVersion = 10;

} // end namespace default_styles
} // end namespace util
} // end namespace mbgl
