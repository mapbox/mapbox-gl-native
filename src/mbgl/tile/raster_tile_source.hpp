#pragma once

#include <mbgl/tile/tile_source.hpp>
#include <mbgl/tile/tile_id.hpp>
#include <mbgl/storage/resource.hpp>

namespace mbgl {

class FileSource;

class RasterTileSource : public TileSource {
public:
    RasterTileSource(const Resource&, FileSource&);
    virtual ~RasterTileSource() = default;

    using Callback = std::function<void(std::exception_ptr,
                                        std::shared_ptr<const std::string>,
                                        optional<Timestamp> modified,
                                        optional<Timestamp> expires)>;

    /*
     * Monitor the tile held by this object for changes. When the tile is loaded for the first time,
     * or updates, the callback is executed. If an error occurs, the first parameter will be set.
     * Otherwise it will be null. If there is no data for the requested tile, the second parameter
     * will be null.
     *
     * To cease monitoring, release the returned Request.
     */
    std::unique_ptr<AsyncRequest> monitorTile(const Callback&);

private:
    const Resource resource;
    FileSource& fileSource;
};

} // namespace mbgl
