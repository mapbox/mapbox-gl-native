#ifndef MBGL_MAP_VECTOR_TILE
#define MBGL_MAP_VECTOR_TILE

#include <mbgl/map/geometry_tile.hpp>
#include <mbgl/geometry/pbf_geometry.hpp>
#include <mbgl/style/filter_expression.hpp>
#include <mbgl/util/ptr.hpp>
#include <mbgl/util/pbf.hpp>

namespace mbgl {

class VectorTileLayer;

class VectorTileFeature : public GeometryTileFeature {
public:
    VectorTileFeature(pbf, const util::ptr<GeometryTileLayer>);

    Geometry nextGeometry();

private:
    pbf geometry_pbf;
    bool multigeometry = false;
};

class FilteredVectorTileLayer : public GeometryFilteredTileLayer {
public:
    FilteredVectorTileLayer(const util::ptr<VectorTileLayer>, const FilterExpression&);

    virtual util::ptr<GeometryTileFeature> nextMatchingFeature();

private:
    pbf feature_pbf;

};

class VectorTileLayer : public GeometryTileLayer, public std::enable_shared_from_this<VectorTileLayer> {
    friend class FilteredVectorTileLayer;
public:
    VectorTileLayer(pbf);

    virtual util::ptr<GeometryFilteredTileLayer> createFilter(const FilterExpression&);
    virtual util::ptr<GeometryTileFeature> nextFeature();

private:
    pbf feature_pbf;
};

class VectorTile : public GeometryTile {
public:
    VectorTile(pbf);

    virtual void logDebug() const;
};

}

#endif
