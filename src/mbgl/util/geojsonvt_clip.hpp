#ifndef MAPBOX_UTIL_GEOJSONVT_CLIP
#define MAPBOX_UTIL_GEOJSONVT_CLIP

#include "geojsonvt_types.hpp"

#include <vector>

namespace mapbox { namespace util { namespace geojsonvt {

class Clip {
public:
    static std::vector<ProjectedFeature> clip(std::vector<ProjectedFeature> features, uint32_t scale, double k1, double k2, uint8_t axis, ProjectedPoint (*intersect)(const ProjectedPoint&, const ProjectedPoint&, double));

private:
    static ProjectedGeometryContainer clipPoints(ProjectedGeometryContainer geometry, double k1, double k2, uint8_t axis);

    static ProjectedGeometryContainer clipGeometry(ProjectedGeometryContainer geometry, double k1, double k2, uint8_t axis, ProjectedPoint (*intersect)(const ProjectedPoint&, const ProjectedPoint&, double), bool closed);

    static ProjectedGeometryContainer newSlice(ProjectedGeometryContainer &slices, ProjectedGeometryContainer &slice, double area, double dist);
};

} /* namespace geojsonvt */ } /* namespace util */ } /* namespace mapbox */

#endif // MAPBOX_UTIL_GEOJSONVT_CLIP
