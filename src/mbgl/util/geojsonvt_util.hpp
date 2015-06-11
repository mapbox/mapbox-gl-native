#ifndef MAPBOX_UTIL_GEOJSONVT_UTIL
#define MAPBOX_UTIL_GEOJSONVT_UTIL

#include <unordered_map>

namespace mapbox { namespace util { namespace geojsonvt {

class Time {
public:
    inline static void time(std::string activity) {
        Time::activities[activity] = clock();
    }

    inline static void timeEnd(std::string activity) {
        printf("%s: %fms\n", activity.c_str(), double(clock() - Time::activities[activity]) / (CLOCKS_PER_SEC / 1000));
    }

private:
    static std::unordered_map<std::string, clock_t> activities;
};

} /* namespace geojsonvt */ } /* namespace util */ } /* namespace mapbox */

#endif // MAPBOX_UTIL_GEOJSONVT_UTIL
