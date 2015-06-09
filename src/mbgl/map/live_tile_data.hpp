#ifndef MBGL_MAP_LIVE_TILE_DATA
#define MBGL_MAP_LIVE_TILE_DATA

#include <mbgl/map/vector_tile_data.hpp>

namespace mbgl {

class AnnotationManager;

class LiveTileData : public VectorTileData {
public:
    LiveTileData(const TileID&,
                 AnnotationManager&,
                 Style&,
                 const SourceInfo&,
                 float angle_,
                 bool collisionDebug_);
    ~LiveTileData();

    bool reparse(std::function<void ()> callback) override;

private:
    AnnotationManager& annotationManager;
};

}

#endif
