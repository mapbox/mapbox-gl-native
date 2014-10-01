#ifndef MBGL_MAP_GEOGRAPHY
#define MBGL_MAP_GEOGRAPHY

namespace mbgl {

typedef struct {
    double latitude;
    double longitude;
} LatLng;

typedef struct {
    double northing;
    double easting;
} ProjectedMeters;

}

#endif
