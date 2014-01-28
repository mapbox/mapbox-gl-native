#ifndef LLMR_MAP_VECTOR_TILE
#define LLMR_MAP_VECTOR_TILE

#include <llmr/util/pbf.hpp>
#include <llmr/style/value.hpp>
#include <vector>
#include <map>
#include <set>

namespace llmr {

class BucketDescription;
class VectorTileLayer;

struct pbf;

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

class VectorTileLayer {
public:
    VectorTileLayer(pbf data);

    const pbf data;
    std::string name;
    uint32_t extent = 4096;
    std::vector<std::string> keys;
    std::vector<Value> values;
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
