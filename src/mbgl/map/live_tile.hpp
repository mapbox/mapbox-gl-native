#ifndef MBGL_MAP_LIVE_TILE
#define MBGL_MAP_LIVE_TILE

#include <map>

#include <mbgl/map/geometry_tile.hpp>
#include <mbgl/map/tile_id.hpp>

namespace mbgl {

class AnnotationManager;

class LiveTile : public GeometryTile {
public:
    LiveTile(const AnnotationManager&, const TileID& id);

    util::ptr<GeometryTileLayer> getLayer(const std::string&) const override;

private:
    const AnnotationManager& manager;
    const TileID id;
};

}

#endif
