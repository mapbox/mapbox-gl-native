#include <mbgl/map/map_context.hpp>
#include <mbgl/map/map_data.hpp>
#include <mbgl/map/environment.hpp>
#include <mbgl/map/sprite.hpp>

#include <mbgl/renderer/painter.hpp>

#include <mbgl/text/glyph_store.hpp>

#include <mbgl/geometry/glyph_atlas.hpp>
#include <mbgl/geometry/sprite_atlas.hpp>
#include <mbgl/geometry/line_atlas.hpp>

#include <mbgl/style/style.hpp>

#include <mbgl/util/std.hpp>
#include <mbgl/util/uv_detail.hpp>
#include <mbgl/util/worker.hpp>
#include <mbgl/util/texture_pool.hpp>

namespace mbgl {

MapContext::MapContext(Environment& env_, MapData& data_)
    : env(env_),
      data(data_),
      glyphStore(util::make_unique<GlyphStore>(env)),
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

util::ptr<Sprite> MapContext::getSprite() {
    assert(Environment::currentlyOn(ThreadType::Map));
    const float pixelRatio = data.getTransformState().getPixelRatio();
    const std::string &sprite_url = style->getSpriteURL();
    if (!sprite || !sprite->hasPixelRatio(pixelRatio)) {
        sprite = Sprite::Create(sprite_url, pixelRatio, env);
    }

    return sprite;
}

}
