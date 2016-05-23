#pragma once

#include <mbgl/tile/tile_source.hpp>
#include <mbgl/tile/file_based_tile_source.hpp>

namespace mbgl {

class FileSource;
class AsyncRequest;

class ImageTileSource final : public FileBasedTileSource<RasterTileSource, ImageTileSource> {
public:
    ImageTileSource(RasterTileData&, const Resource&, FileSource&);
    virtual ~ImageTileSource() = default;

    static std::shared_ptr<const std::string> parseData(std::shared_ptr<const std::string> data) {
        return data;
    }

};

} // namespace mbgl
