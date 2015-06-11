#ifndef MAPBOX_UTIL_GEOJSONVT_TILE
#define MAPBOX_UTIL_GEOJSONVT_TILE

#include "geojsonvt_types.hpp"

namespace mapbox { namespace util { namespace geojsonvt {

class Tile {
public:
    static Tile createTile(std::vector<ProjectedFeature> &features, uint32_t z2, uint32_t tx, uint32_t ty, double tolerance, uint16_t extent, bool noSimplify);

    static void addFeature(Tile &tile, ProjectedFeature &feature, uint32_t z2, uint32_t tx, uint32_t ty, double tolerance, uint16_t extent, bool noSimplify);

    inline operator bool() const { return this->numPoints > 0; }

private:
    static TilePoint transformPoint(const ProjectedPoint &p, uint32_t z2, uint32_t tx, uint32_t ty, uint16_t extent);

public:
    std::vector<TileFeature> features;
    uint32_t numPoints = 0;
    uint32_t numSimplified = 0;
    uint32_t numFeatures = 0;
    std::vector<ProjectedFeature> source;
};

} /* namespace geojsonvt */ } /* namespace util */ } /* namespace mapbox */

#endif // MAPBOX_UTIL_GEOJSONVT_TILE
