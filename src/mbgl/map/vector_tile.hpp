#ifndef MBGL_MAP_VECTOR_TILE
#define MBGL_MAP_VECTOR_TILE

#include <mbgl/map/geometry_tile.hpp>
#include <mbgl/style/filter_expression.hpp>
#include <mbgl/util/pbf.hpp>

namespace mbgl {

class VectorTileLayer;

class VectorTileFeature : public GeometryTileFeature<pbf> {
public:
    VectorTileFeature(pbf, const VectorTileLayer&);

private:
    const pbf feature_pbf;
};

std::ostream& operator<<(std::ostream&, const GeometryTileFeature<pbf>&);

class VectorTileTagExtractor : public GeometryTileTagExtractor<pbf> {
public:
    VectorTileTagExtractor(const VectorTileLayer&);

    void setTags(const pbf&);
    mapbox::util::optional<Value> getValue(const std::string &key) const;

private:
    pbf tags_pbf;
};

class FilteredVectorTileLayer : public GeometryFilteredTileLayer<pbf> {
public:
    class iterator : public GeometryFilteredTileLayer<pbf>::iterator {
    public:
        iterator(const FilteredVectorTileLayer&, const pbf&);
        void operator++();
        bool operator!=(const iterator& other) const;
        const pbf& operator*() const;

    private:
        pbf feature_pbf;
        pbf data_pbf;
    };

public:
    FilteredVectorTileLayer(const VectorTileLayer&, const FilterExpression&);

    iterator begin() const;
    iterator end() const;
};

class VectorTileLayer : public GeometryTileLayer {
public:
    VectorTileLayer(pbf);

public:
    const pbf layer_pbf;
};

class VectorTile : public GeometryTile {
public:
    VectorTile(pbf);
};

}

#endif
