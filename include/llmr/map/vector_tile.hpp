#ifndef LLMR_MAP_VECTOR_TILE
#define LLMR_MAP_VECTOR_TILE

#include <llmr/util/pbf.hpp>
#include <llmr/util/vec.hpp>
#include <llmr/style/value.hpp>
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

class VectorTilePlacement {
public:
    VectorTilePlacement(uint32_t face, uint32_t glyph, uint32_t x, uint32_t y);

    uint32_t face = 0;
    uint32_t glyph = 0;
    uint32_t x = 0;
    uint32_t y = 0;
};

std::ostream& operator<<(std::ostream&, const VectorTilePlacement& placement);


class VectorTileLayer {
public:
    VectorTileLayer(pbf data);

    const pbf data;
    std::string name;
    uint32_t extent = 4096;
    std::vector<std::string> keys;
    std::vector<Value> values;
    std::vector<std::string> faces;
    std::map<std::string, std::map<Value, std::vector<VectorTilePlacement>>> shaping;
};

class VectorTileGlyph {
public:
    VectorTileGlyph(pbf data);

    uint32_t id = 0;

    // A signed distance field of the glyph with a border of 3 pixels.
    std::string bitmap;

    // Glyph metrics.
    uint32_t width = 0;
    uint32_t height = 0;
    int32_t left = 0;
    int32_t top = 0;
    uint32_t advance = 0;
};

std::ostream& operator<<(std::ostream&, const VectorTileGlyph& glyph);

class VectorTileFace {
public:
    VectorTileFace(pbf data);

    std::string name;
    std::string family;
    std::string style;
    std::vector<VectorTileGlyph> glyphs;
};

std::ostream& operator<<(std::ostream&, const VectorTileFace& face);

class VectorTile {
public:
    VectorTile();
    VectorTile(pbf data);
    VectorTile& operator=(VectorTile&& other);

    std::map<std::string, const VectorTileLayer> layers;
    std::map<std::string, const VectorTileFace> faces;
};



}

#endif
