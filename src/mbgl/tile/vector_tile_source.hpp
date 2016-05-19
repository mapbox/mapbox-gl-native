#pragma once

#include <mbgl/tile/tile_source.hpp>
#include <mbgl/tile/tile_id.hpp>
#include <mbgl/storage/resource.hpp>

namespace mbgl {

class FileSource;

class VectorTileSource : public GeometryTileSource {
public:
    VectorTileSource(const Resource&, FileSource&);
    virtual ~VectorTileSource() = default;

    std::unique_ptr<AsyncRequest> monitorTile(const Callback&) override;

private:
    const Resource resource;
    FileSource& fileSource;
};

} // namespace mbgl
