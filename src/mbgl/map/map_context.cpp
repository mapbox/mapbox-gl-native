#include <mbgl/map/map_context.hpp>
#include <mbgl/map/environment.hpp>

#include <mbgl/renderer/painter.hpp>

#include <mbgl/text/glyph_store.hpp>

#include <mbgl/geometry/glyph_atlas.hpp>
#include <mbgl/geometry/sprite_atlas.hpp>
#include <mbgl/geometry/line_atlas.hpp>

#include <mbgl/util/std.hpp>
#include <mbgl/util/uv_detail.hpp>
#include <mbgl/util/worker.hpp>
#include <mbgl/util/texture_pool.hpp>

namespace mbgl {

MapContext::MapContext(Environment& env)
    : glyphStore(util::make_unique<GlyphStore>(env)),
      glyphAtlas(util::make_unique<GlyphAtlas>(1024, 1024)),
      spriteAtlas(util::make_unique<SpriteAtlas>(512, 512)),
      lineAtlas(util::make_unique<LineAtlas>(512, 512)),
      texturePool(util::make_unique<TexturePool>()),
      painter(util::make_unique<Painter>(*spriteAtlas, *glyphAtlas, *lineAtlas)) {
}

Worker& MapContext::getWorker() {
    assert(workers);
    return *workers;
}

}