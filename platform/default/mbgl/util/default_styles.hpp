#pragma once

#include <vector>
#include <string>

#ifndef MGL_EXTERN
#ifdef __cplusplus
#define MGL_EXTERN extern "C" __attribute__((visibility ("default")))
#else
#define MGL_EXTERN extern __attribute__((visibility ("default")))
#endif
#endif

namespace mbgl {
namespace util {
namespace default_styles {

struct DefaultStyle {
    const char* url;
    const char* name;
};

MGL_EXTERN const DefaultStyle streets;
MGL_EXTERN const DefaultStyle outdoors;
MGL_EXTERN const DefaultStyle light;
MGL_EXTERN const DefaultStyle dark;
MGL_EXTERN const DefaultStyle satellite;
MGL_EXTERN const DefaultStyle satelliteStreets;

const DefaultStyle orderedStyles[] = {
    streets, outdoors, light, dark, satellite, satelliteStreets,
};
const size_t numOrderedStyles = sizeof(orderedStyles) / sizeof(DefaultStyle);

static const unsigned currentVersion = 9;

} // end namespace default_styles
} // end namespace util
} // end namespace mbgl
