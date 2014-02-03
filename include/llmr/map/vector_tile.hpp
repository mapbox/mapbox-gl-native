#ifndef LLMR_MAP_VECTOR_TILE
#define LLMR_MAP_VECTOR_TILE

#include <llmr/util/pbf.hpp>
#include <llmr/style/value.hpp>
#include <vector>
#include <map>
#include <set>
#include <limits>

namespace llmr {

class BucketDescription;
class VectorTileLayer;

struct pbf;

struct Coordinate {
    struct null {};

    Coordinate() : x(0), y(0) {}
    Coordinate(int16_t x, int16_t y) : x(x), y(y) {}
    Coordinate(null) : x(std::numeric_limits<int16_t>::min()), y(std::numeric_limits<int16_t>::min()) {}
    int16_t x;
    int16_t y;

    inline bool operator==(const Coordinate& other) const {
        return x == other.x && y == other.y;
    }

    inline operator bool() const {
        return x != std::numeric_limits<int16_t>::min() && y != std::numeric_limits<int16_t>::min();
    }
};

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
