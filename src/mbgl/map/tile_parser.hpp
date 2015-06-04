#ifndef MBGL_MAP_TILE_PARSER
#define MBGL_MAP_TILE_PARSER

#include <mbgl/map/geometry_tile.hpp>
#include <mbgl/map/vector_tile.hpp>
#include <mbgl/style/filter_expression.hpp>
#include <mbgl/style/class_properties.hpp>
#include <mbgl/style/style_bucket.hpp>
#include <mbgl/text/glyph.hpp>
#include <mbgl/util/ptr.hpp>
#include <mbgl/util/noncopyable.hpp>

#include <cstdint>
#include <iosfwd>
#include <string>

namespace mbgl {

class Bucket;
class FontStack;
class Style;
class StyleBucket;
class StyleLayoutFill;
class StyleLayoutRaster;
class StyleLayoutLine;
class StyleLayoutSymbol;
class TileWorker;

class TileParser : private util::noncopyable {
public:
    TileParser(const GeometryTile&, TileWorker&, Style&);
    ~TileParser();

public:
    void parse();
    inline bool isPartialParse() const {
        return partialParse;
    }

private:
    bool obsolete() const;

    std::unique_ptr<Bucket> createBucket(const StyleBucket&);
    std::unique_ptr<Bucket> createFillBucket(const GeometryTileLayer&, const StyleBucket&);
    std::unique_ptr<Bucket> createLineBucket(const GeometryTileLayer&, const StyleBucket&);
    std::unique_ptr<Bucket> createSymbolBucket(const GeometryTileLayer&, const StyleBucket&);

    template <class Bucket>
    void addBucketGeometries(Bucket&, const GeometryTileLayer&, const FilterExpression&);

    const GeometryTile& geometryTile;
    TileWorker& tile;

    // Cross-thread shared data.
    Style& style;

    bool partialParse;
};

}

#endif
