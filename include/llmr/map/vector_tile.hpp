#ifndef LLMR_MAP_VECTOR_TILE
#define LLMR_MAP_VECTOR_TILE

#include <llmr/util/pbf.hpp>
#include <llmr/util/vec.hpp>
#include <llmr/style/value.hpp>
#include <llmr/text/glyph.hpp>
#include <vector>
#include <map>
#include <set>
#include <limits>

namespace llmr {

class BucketDescription;
class VectorTileLayer;

struct pbf;

enum class FeatureType {
    Unknown = 0,
    Point = 1,
    LineString = 2,
    Polygon = 3
};

std::ostream& operator<<(std::ostream&, const FeatureType& type);

class VectorTileFeature {
public:
    VectorTileFeature(pbf feature, const VectorTileLayer& layer);

    uint64_t id = 0;
    FeatureType type = FeatureType::Unknown;
    std::map<std::string, Value> properties;
    pbf geometry;
};

std::ostream& operator<<(std::ostream&, const VectorTileFeature& feature);

/*
 * Allows iterating over the features of a VectorTileLayer using a
 * BucketDescription as filter. Only features matching the descriptions will
 * be returned (as pbf).
 */
class FilteredVectorTileLayer {
public:
    class iterator {
    public:
        iterator(const FilteredVectorTileLayer& filter, const pbf& data);
        void operator++();
        bool operator!=(const iterator& other) const;
        const pbf& operator*() const;

    private:
        const FilteredVectorTileLayer& filter;
        bool valid = false;
        pbf feature;
        pbf data;
    };

public:
    FilteredVectorTileLayer(const VectorTileLayer& layer, const BucketDescription& bucket_desc);

    iterator begin() const;
    iterator end() const;

private:
    const VectorTileLayer& layer;
    const BucketDescription& bucket_desc;
    int32_t key = -1;
    std::set<uint32_t> values;
};

std::ostream& operator<<(std::ostream&, const GlyphPlacement& placement);

class VectorTileLayer {
public:
    VectorTileLayer(pbf data);

    const pbf data;
    std::string name;
    uint32_t extent = 4096;
    std::vector<std::string> keys;
    std::vector<Value> values;
    std::map<std::string, std::map<Value, Shaping>> shaping;
};

class VectorTile {
public:
    VectorTile();
    VectorTile(pbf data);
    VectorTile& operator=(VectorTile&& other);

    std::map<std::string, const VectorTileLayer> layers;
};



}

#endif
