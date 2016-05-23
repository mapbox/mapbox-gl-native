#pragma once

#include <mbgl/tile/tile_source.hpp>
#include <mbgl/tile/file_based_tile_source.hpp>

#include <memory>

namespace mbgl {

class GeometryTile;

class VectorTileSource final : public FileBasedTileSource<GeometryTileSource, VectorTileSource> {
public:
    VectorTileSource(GeometryTileData&, const Resource&, FileSource&);
    virtual ~VectorTileSource() = default;

    static std::unique_ptr<GeometryTile> parseData(std::shared_ptr<const std::string>);
};

} // namespace mbgl
