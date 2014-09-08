#ifndef MBGL_GEOMETRY_LAT_LNG
#define MBGL_GEOMETRY_LAT_LNG

namespace mbgl {

class LatLng {
public:
    double lat = 0;
    double lng = 0;

    LatLng() {}
    LatLng(double lat, double lng) : lat(lat), lng(lng) {}
};

}

#endif
