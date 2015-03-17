#ifndef MBGL_MAP_LIVE_TILE_DATA
#define MBGL_MAP_LIVE_TILE_DATA

#include <mbgl/map/vector_tile_data.hpp>

namespace mbgl {

class AnnotationManager;

class LiveTileData : public VectorTileData {
public:
    LiveTileData(Tile::ID const&,
                 util::ptr<AnnotationManager>,
                 float mapMaxZoom,
                 util::ptr<Style>,
                 GlyphAtlas&,
                 GlyphStore&,
                 SpriteAtlas&,
                 util::ptr<Sprite>,
                 const SourceInfo&,
                 Environment&);
    ~LiveTileData();

    void parse() override;

private:
    util::ptr<AnnotationManager> annotationManager;
};

}

#endif
