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

constexpr const DefaultStyle streets          = { "mapbox://styles/mapbox/streets-v10",           "Streets",           10 };
constexpr const DefaultStyle outdoors         = { "mapbox://styles/mapbox/outdoors-v10",          "Outdoors",          10 };
constexpr const DefaultStyle light            = { "mapbox://styles/mapbox/light-v9",              "Light",              9 };
constexpr const DefaultStyle dark             = { "mapbox://styles/mapbox/dark-v9",               "Dark",               9 };
constexpr const DefaultStyle satellite        = { "mapbox://styles/mapbox/satellite-v9",          "Satellite",          9 };
constexpr const DefaultStyle satelliteStreets = { "mapbox://styles/mapbox/satellite-streets-v10", "Satellite Streets", 10 };
constexpr const DefaultStyle trafficDay       = { "mapbox://styles/mapbox/traffic-day-v2",        "Traffic Day",        2 };
constexpr const DefaultStyle trafficNight     = { "mapbox://styles/mapbox/traffic-night-v2",      "Traffic Night",      2 };

const DefaultStyle orderedStyles[] = {
    streets, outdoors, light, dark, satellite, satelliteStreets,
    trafficDay, trafficNight,
};
const size_t numOrderedStyles = sizeof(orderedStyles) / sizeof(DefaultStyle);

} // end namespace default_styles
} // end namespace util
} // end namespace mbgl
