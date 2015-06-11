#include "geojsonvt_tile.hpp"

namespace mapbox { namespace util { namespace geojsonvt {

Tile Tile::createTile(std::vector<ProjectedFeature> &features, uint32_t z2, uint32_t tx, uint32_t ty, double tolerance, uint16_t extent, bool noSimplify) {

    Tile tile;

    for (size_t i = 0; i < features.size(); ++i) {
        tile.numFeatures++;
        addFeature(tile, features[i], z2, tx, ty, tolerance, extent, noSimplify);
    }

    return std::move(tile);
}

void Tile::addFeature(Tile &tile, ProjectedFeature &feature, uint32_t z2, uint32_t tx, uint32_t ty, double tolerance, uint16_t extent, bool noSimplify) {

    ProjectedGeometryContainer *geom = &(feature.geometry.get<ProjectedGeometryContainer>());
    ProjectedFeatureType type = feature.type;
    std::vector<TileGeometry> transformed;
    double sqTolerance = tolerance * tolerance;
    ProjectedGeometryContainer ring;

    if (type == ProjectedFeatureType::Point) {
        for (size_t i = 0; i < geom->members.size(); ++i) {
            ProjectedPoint *p = &(geom->members[i].get<ProjectedPoint>());
            transformed.push_back(transformPoint(*p, z2, tx, ty, extent));
            tile.numPoints++;
            tile.numSimplified++;
        }
    } else {
        for (size_t i = 0; i < geom->members.size(); ++i) {
            ring = geom->members[i].get<ProjectedGeometryContainer>();

            if (!noSimplify && ((type == ProjectedFeatureType::LineString && ring.dist < tolerance) ||
                (type == ProjectedFeatureType::Polygon && ring.area < sqTolerance))) {
                tile.numPoints += ring.members.size();
                continue;
            }

            TileRing transformedRing;

            for (size_t j = 0; j < ring.members.size(); ++j) {
                ProjectedPoint *p = &(ring.members[j].get<ProjectedPoint>());
                if (noSimplify || p->z > sqTolerance) {
                    TilePoint transformedPoint = transformPoint(*p, z2, tx, ty, extent);
                    transformedRing.points.push_back(transformedPoint);
                    tile.numSimplified++;
                }
                tile.numPoints++;
            }

            transformed.push_back(transformedRing);
        }
    }

    if (transformed.size()) {
        tile.features.push_back(TileFeature(transformed, type, Tags(feature.tags)));
    }

}

TilePoint Tile::transformPoint(const ProjectedPoint &p, uint32_t z2, uint32_t tx, uint32_t ty, uint16_t extent) {

    int16_t x = extent * (p.x * z2 - tx);
    int16_t y = extent * (p.y * z2 - ty);

    return TilePoint(x, y);
}

} /* namespace geojsonvt */ } /* namespace util */ } /* namespace mapbox */
