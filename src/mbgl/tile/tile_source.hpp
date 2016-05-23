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
public:
    using data_type = GeometryTileData;

protected:
    GeometryTileSource(data_type& tileData_) : tileData(tileData_) {}

public:
    virtual ~GeometryTileSource() = default;

protected:
    data_type& tileData;
};

class RasterTileSource : public TileSource {
public:
    using data_type = RasterTileData;

protected:
    RasterTileSource(data_type& tileData_) : tileData(tileData_) {};

public:
    virtual ~RasterTileSource() = default;

protected:
    data_type& tileData;
};

} // namespace mbgl
