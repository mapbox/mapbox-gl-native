#pragma once

#include <mbgl/tile/tile_source.hpp>
#include <mbgl/tile/tile_id.hpp>

#include <memory>

namespace mbgl {

class AnnotationManager;
class GeometryTile;

class AnnotationTileSource : public GeometryTileSource {
public:
    AnnotationTileSource(GeometryTileData&, const OverscaledTileID&, AnnotationManager&);
    ~AnnotationTileSource();

    void update(std::unique_ptr<GeometryTile>);

    OverscaledTileID tileID;

private:
    AnnotationManager& annotationManager;
};

} // namespace mbgl
