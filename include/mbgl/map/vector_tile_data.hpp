#ifndef MBGL_MAP_VECTOR_TILE_DATA
#define MBGL_MAP_VECTOR_TILE_DATA

#include <mbgl/map/tile_data.hpp>


#include <mbgl/renderer/bucket.hpp>

#include <mbgl/geometry/vertex_buffer.hpp>
#include <mbgl/geometry/elements_buffer.hpp>
#include <mbgl/geometry/fill_buffer.hpp>
#include <mbgl/geometry/line_buffer.hpp>
#include <mbgl/geometry/icon_buffer.hpp>
#include <mbgl/geometry/text_buffer.hpp>
#include <mbgl/map/tile_parser.hpp>

#include <unordered_map>

namespace mbgl {


class VectorTileData : public TileData {
    friend class TileParser;

public:
    VectorTileData(Tile::ID id, Map &map, const SourceInfo &source);
    ~VectorTileData();

    virtual void beforeParse();
    virtual void parse();
    virtual void afterParse();
    virtual void render(Painter &painter, std::shared_ptr<StyleLayer> layer_desc);
    virtual bool hasData(std::shared_ptr<StyleLayer> layer_desc) const;

protected:
    // Holds the actual geometries in this tile.
    FillVertexBuffer fillVertexBuffer;
    LineVertexBuffer lineVertexBuffer;
    IconVertexBuffer iconVertexBuffer;
    TextVertexBuffer textVertexBuffer;

    TriangleElementsBuffer triangleElementsBuffer;
    LineElementsBuffer lineElementsBuffer;
    PointElementsBuffer pointElementsBuffer;

    // Holds the buckets of this tile.
    // They contain the location offsets in the buffers stored above
    std::unordered_map<std::string, std::unique_ptr<Bucket>> buckets;

    std::unique_ptr<TileParser> parser;
};

}

#endif
