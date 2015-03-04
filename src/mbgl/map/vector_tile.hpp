#ifndef MBGL_MAP_VECTOR_TILE
#define MBGL_MAP_VECTOR_TILE

#include <mbgl/map/geometry_tile.hpp>
#include <mbgl/geometry/pbf_geometry.hpp>
#include <mbgl/style/filter_expression.hpp>
#include <mbgl/util/pbf.hpp>

namespace mbgl {

class VectorTileLayer;

class VectorTileFeature : public GeometryTileFeature {
public:
    VectorTileFeature(pbf, const GeometryTileLayer&);

    Geometry nextGeometry();

private:
    pbf geometry_pbf;
    bool multigeometry = false;
};

class FilteredVectorTileLayer : public GeometryFilteredTileLayer {
public:
    FilteredVectorTileLayer(const VectorTileLayer&, const FilterExpression&);

    GeometryTileFeature nextMatchingFeature();

private:
    pbf feature_pbf;

};

class VectorTileLayer : public GeometryTileLayer {
    friend class FilteredVectorTileLayer;
public:
    VectorTileLayer(pbf);

    GeometryTileFeature nextFeature();

private:
    pbf feature_pbf;
};

class VectorTile : public GeometryTile {
public:
    VectorTile(pbf);
};

}

#endif
