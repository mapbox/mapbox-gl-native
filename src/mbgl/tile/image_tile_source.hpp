#pragma once

#include <mbgl/tile/tile_source.hpp>
#include <mbgl/tile/tile_id.hpp>
#include <mbgl/storage/resource.hpp>

namespace mbgl {

class FileSource;
class AsyncRequest;

class ImageTileSource : public RasterTileSource {
public:
    ImageTileSource(RasterTileData&, const Resource&, FileSource&);
    virtual ~ImageTileSource() = default;

private:
    const Resource resource;
    FileSource& fileSource;
    std::unique_ptr<AsyncRequest> request;
};

} // namespace mbgl
