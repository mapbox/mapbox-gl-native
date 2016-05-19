#pragma once

#include <mbgl/tile/tile_source.hpp>
#include <mbgl/tile/tile_id.hpp>

namespace mbgl {

class FileSource;

class VectorTileSource : public GeometryTileSource {
public:
    VectorTileSource(const OverscaledTileID&,
                     float pixelRatio,
                     const std::string& urlTemplate,
                     FileSource&);

    std::unique_ptr<AsyncRequest> monitorTile(const GeometryTileSource::Callback&) override;

private:
    OverscaledTileID tileID;
    float pixelRatio;
    std::string urlTemplate;
    FileSource& fileSource;
};

} // namespace mbgl
