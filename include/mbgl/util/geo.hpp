#ifndef MBGL_UTIL_GEO
#define MBGL_UTIL_GEO

namespace mbgl {

struct LatLng {
    double latitude = 0;
    double longitude = 0;

    inline LatLng(double lat = 0, double lon = 0)
        : latitude(lat), longitude(lon) {}
};

struct ProjectedMeters {
    double northing = 0;
    double easting = 0;

    inline ProjectedMeters(double n = 0, double e = 0)
        : northing(n), easting(e) {}
};

struct BoundingBox {
    LatLng sw = {90, 180};
    LatLng ne = {-90, -180};

    inline BoundingBox(LatLng sw_ = {90, 180}, LatLng ne_ = {-90, -180})
        : sw(sw_), ne(ne_) {}
};

}

#endif
