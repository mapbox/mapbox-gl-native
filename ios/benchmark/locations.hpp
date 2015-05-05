#ifndef MBGL_BENCH_LOCATIONS
#define MBGL_BENCH_LOCATIONS

#include <vector>
#include <string>

namespace mbgl {
namespace bench {

struct Location {
    const std::string name;
    const double longitude;
    const double latitude;
    const double zoom;
    const double bearing;
};

extern const std::vector<Location> locations;

}
}

#endif