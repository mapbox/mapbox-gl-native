#ifndef MAPBOX_UTIL_GEOJSONVT_SIMPLIFY
#define MAPBOX_UTIL_GEOJSONVT_SIMPLIFY

#include "geojsonvt_types.hpp"

namespace mapbox { namespace util { namespace geojsonvt {

class Simplify {
public:
    static void simplify(ProjectedGeometryContainer &points, double tolerance);

private:
    static double getSqSegDist(const ProjectedPoint &p, const ProjectedPoint &a, const ProjectedPoint &b);
};

} /* namespace geojsonvt */ } /* namespace util */ } /* namespace mapbox */

#endif // MAPBOX_UTIL_GEOJSONVT_SIMPLIFY
