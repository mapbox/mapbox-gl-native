#ifndef MBGL_MAP_GEOGRAPHY
#define MBGL_MAP_GEOGRAPHY

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
