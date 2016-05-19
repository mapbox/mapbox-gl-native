#pragma once

#include <mbgl/tile/tile_source.hpp>
#include <mbgl/tile/tile_id.hpp>

#include <memory>

namespace mbgl {

class AnnotationManager;

class AnnotationTileSource : public GeometryTileSource {
public:
    AnnotationTileSource(const OverscaledTileID&, AnnotationManager&);
    ~AnnotationTileSource();

    void update(std::unique_ptr<GeometryTile>);
    std::unique_ptr<AsyncRequest> monitorTile(const GeometryTileSource::Callback&) override;

    OverscaledTileID tileID;

private:
    AnnotationManager& annotationManager;
    GeometryTileSource::Callback callback;
};

} // namespace mbgl
