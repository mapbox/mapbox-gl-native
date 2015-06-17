#ifndef MBGL_MAP_LIVE_TILE_DATA
#define MBGL_MAP_LIVE_TILE_DATA

#include <mbgl/map/vector_tile_data.hpp>

namespace mbgl {

class AnnotationManager;

class LiveTileData : public VectorTileData {
public:
    LiveTileData(const TileID&,
                 AnnotationManager&,
                 const std::vector<util::ptr<StyleLayer>>&,
                 Worker&,
                 GlyphAtlas&,
                 GlyphStore&,
                 SpriteAtlas&,
                 util::ptr<Sprite>,
                 const SourceInfo&,
                 float angle_,
                 bool collisionDebug_);
    ~LiveTileData();

    void parse() override;

private:
    AnnotationManager& annotationManager;
};

}

#endif
