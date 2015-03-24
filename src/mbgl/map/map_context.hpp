#ifndef MBGL_MAP_MAP_CONTEXT
#define MBGL_MAP_MAP_CONTEXT

#include <mbgl/util/ptr.hpp>

#include <set>

namespace mbgl {

class Environment;
class GlyphStore;
class GlyphAtlas;
class SpriteAtlas;
class LineAtlas;
class TexturePool;
class Painter;
class Sprite;
class Style;
class Worker;

class MapContext {
public:
    MapContext(Environment&);

    Worker& getWorker();

public:
    std::unique_ptr<Worker> workers;
    const std::unique_ptr<GlyphStore> glyphStore;
    const std::unique_ptr<GlyphAtlas> glyphAtlas;
    const std::unique_ptr<SpriteAtlas> spriteAtlas;
    const std::unique_ptr<LineAtlas> lineAtlas;
    const std::unique_ptr<TexturePool> texturePool;
    const std::unique_ptr<Painter> painter;
    util::ptr<Sprite> sprite;
    util::ptr<Style> style;
};

}
#endif