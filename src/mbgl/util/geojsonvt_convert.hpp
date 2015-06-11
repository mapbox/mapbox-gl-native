#ifndef MAPBOX_UTIL_GEOJSONVT_CONVERT
#define MAPBOX_UTIL_GEOJSONVT_CONVERT

#include "geojsonvt_types.hpp"

#include <vector>

namespace mapbox { namespace util { namespace geojsonvt {

class Convert {
public:
    static std::vector<ProjectedFeature> convert(const JSDocument &data, double tolerance);

    static ProjectedFeature create(Tags tags, ProjectedFeatureType type, ProjectedGeometry geometry);

    static ProjectedGeometryContainer project(const std::vector<LonLat> &lonlats, double tolerance = 0);

private:
    static void convertFeature(std::vector<ProjectedFeature> &features, const JSValue &feature, double tolerance);

    static ProjectedPoint projectPoint(const LonLat &p);

    static void calcSize(ProjectedGeometryContainer &geometryContainer);

    static void calcBBox(ProjectedFeature &feature);

    static void calcRingBBox(ProjectedPoint &minPoint, ProjectedPoint &maxPoint, const ProjectedGeometryContainer &geometry);
};

} /* namespace geojsonvt */ } /* namespace util */ } /* namespace mapbox */

#endif // MAPBOX_UTIL_GEOJSONVT_CONVERT
