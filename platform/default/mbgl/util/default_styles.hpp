#pragma once

#include <vector>
#include <string>

#ifndef MGL_EXPORT
#define MGL_EXPORT __attribute__((visibility ("default")))
#endif

namespace mbgl {
namespace util {
namespace default_styles {

struct DefaultStyle {
    const char* url;
    const char* name;
};

extern MGL_EXPORT const DefaultStyle streets;
extern MGL_EXPORT const DefaultStyle outdoors;
extern MGL_EXPORT const DefaultStyle light;
extern MGL_EXPORT const DefaultStyle dark;
extern MGL_EXPORT const DefaultStyle satellite;
extern MGL_EXPORT const DefaultStyle satelliteStreets;

const DefaultStyle orderedStyles[] = {
    streets, outdoors, light, dark, satellite, satelliteStreets,
};
const size_t numOrderedStyles = sizeof(orderedStyles) / sizeof(DefaultStyle);

static const unsigned currentVersion = 9;

} // end namespace default_styles
} // end namespace util
} // end namespace mbgl
