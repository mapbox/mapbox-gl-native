#ifndef MBGL_UTIL_GEO
#define MBGL_UTIL_GEO

namespace mbgl {

struct LatLng {
    double latitude;
    double longitude;
};

struct ProjectedMeters {
    double northing;
    double easting;
};

}

#endif
