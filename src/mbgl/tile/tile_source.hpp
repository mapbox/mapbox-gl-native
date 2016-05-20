#pragma once

#include <mbgl/util/noncopyable.hpp>

namespace mbgl {

class TileSource : private util::noncopyable {
public:
    virtual ~TileSource() = default;
};

class GeometryTileData;
class RasterTileData;

class GeometryTileSource : public TileSource {
protected:
    GeometryTileSource(GeometryTileData& tileData_) : tileData(tileData_) {}

public:
    virtual ~GeometryTileSource() = default;

protected:
    GeometryTileData& tileData;
};

class RasterTileSource : public TileSource {
protected:
    RasterTileSource(RasterTileData& tileData_) : tileData(tileData_) {};

public:
    virtual ~RasterTileSource() = default;

protected:
    RasterTileData& tileData;
};

} // namespace mbgl
