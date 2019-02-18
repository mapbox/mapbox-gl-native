#pragma once

#include <vector>
#include <string>

namespace mbgl {
namespace util {
namespace default_styles {

struct DefaultStyle {
    const char* url;
    const char* name;
    const unsigned currentVersion;
};

constexpr const DefaultStyle streets          = { "mapbox://styles/mapbox/streets-v11",           "Streets",           11 };
constexpr const DefaultStyle outdoors         = { "mapbox://styles/mapbox/outdoors-v11",          "Outdoors",          11 };
constexpr const DefaultStyle light            = { "mapbox://styles/mapbox/light-v10",             "Light",             10 };
constexpr const DefaultStyle dark             = { "mapbox://styles/mapbox/dark-v10",              "Dark",              10 };
constexpr const DefaultStyle satellite        = { "mapbox://styles/mapbox/satellite-v9",          "Satellite",          9 };
constexpr const DefaultStyle satelliteStreets = { "mapbox://styles/mapbox/satellite-streets-v11", "Satellite Streets", 11 };

const DefaultStyle orderedStyles[] = {
    streets, outdoors, light, dark, satellite, satelliteStreets,
};
const size_t numOrderedStyles = sizeof(orderedStyles) / sizeof(DefaultStyle);

} // end namespace default_styles
} // end namespace util
} // end namespace mbgl
