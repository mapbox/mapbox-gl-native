#include <mbgl/style/style.hpp>
#include <mbgl/map/sprite.hpp>
#include <mbgl/map/source.hpp>
#include <mbgl/style/style_layer_group.hpp>
#include <mbgl/style/style_parser.hpp>
#include <mbgl/style/style_bucket.hpp>
#include <mbgl/util/constants.hpp>
#include <mbgl/util/error.hpp>
#include <mbgl/util/std.hpp>
#include <mbgl/util/uv_detail.hpp>
#include <mbgl/platform/log.hpp>
#include <mbgl/text/glyph_store.hpp>
#include <mbgl/geometry/glyph_atlas.hpp>
#include <mbgl/geometry/sprite_atlas.hpp>
#include <mbgl/geometry/line_atlas.hpp>
#include <mbgl/util/mapbox.hpp>
#include <mbgl/map/map.hpp>

#include <csscolorparser/csscolorparser.hpp>

#include <rapidjson/document.h>

#include <algorithm>

namespace mbgl {

Style::Style(Environment& env)
    : glyphAtlas(util::make_unique<GlyphAtlas>(1024, 1024)),
    spriteAtlas(util::make_unique<SpriteAtlas>(512, 512)),
    lineAtlas(util::make_unique<LineAtlas>(512, 512)),
    mtx(util::make_unique<uv::rwlock>()),
    glyphStore(util::make_unique<GlyphStore>(env))
{
}

// Note: This constructor is seemingly empty, but we need to declare it anyway
// because this file includes uv_detail.hpp, which has the declarations necessary
// for deleting the std::unique_ptr<uv::rwlock>.
Style::~Style() {
}

void Style::updateSources(Map& map,
                          Environment& env,
                          uv::worker& worker,
                          TexturePool& texturePool,
                          std::function<void()> callback) {
    // First, disable all existing sources.
    for (const auto& source : activeSources) {
        source->enabled = false;
    }

    // Then, reenable all of those that we actually use when drawing this layer.
    if (!layers) {
        return;
    }
    for (const util::ptr<StyleLayer> &layer : layers->layers) {
        if (!layer) continue;
        if (layer->bucket && layer->bucket->style_source) {
            (*activeSources.emplace(layer->bucket->style_source).first)->enabled = true;
        }
    }

    // Then, construct or destroy the actual source object, depending on enabled state.
    for (const auto& source : activeSources) {
        if (source->enabled) {
            if (!source->source) {
                source->source = std::make_shared<Source>(source->info);
                source->source->load(map, env);
            }
        } else {
            source->source.reset();
        }
    }

    // Finally, remove all sources that are disabled.
    util::erase_if(activeSources, [](util::ptr<StyleSource> source){
        return !source->enabled;
    });

    // Allow the sprite atlas to potentially pull new sprite images if needed.
    const float pixelRatio = map.getState().getPixelRatio();
    if (!sprite || sprite->pixelRatio != pixelRatio) {
        sprite = Sprite::Create(sprite_url, pixelRatio, env);
        spriteAtlas->resize(pixelRatio);
        spriteAtlas->setSprite(sprite);
    }

    glyphStore->setURL(util::mapbox::normalizeGlyphsURL(glyph_url, map.getAccessToken()));

    for (const auto &source : activeSources) {
        source->source->update(map, env, worker, shared_from_this(),
                               *glyphAtlas, *glyphStore,
                               *spriteAtlas, sprite,
                               texturePool, callback);
    }
}

void Style::updateProperties(float z, std::chrono::steady_clock::time_point now) {
    uv::writelock lock(mtx);

    zoomHistory.update(z, now);

    if (layers) {
        layers->updateProperties(z, now, zoomHistory);
    }

    // Apply transitions after the first time.
    if (!initial_render_complete) {
        initial_render_complete = true;
        return;
    }
}

void Style::setDefaultTransitionDuration(std::chrono::steady_clock::duration duration) {
    defaultTransition.duration = duration;
}

void Style::cascadeClasses(const std::vector<std::string>& classes) {
    if (layers) {
        layers->setClasses(classes, std::chrono::steady_clock::now(), defaultTransition);
    }
}

bool Style::hasTransitions() const {
    if (layers) {
        if (layers->hasTransitions()) {
            return true;
        }
    }
    return false;
}

void Style::loadJSON(const uint8_t *const data) {
    uv::writelock lock(mtx);

    rapidjson::Document doc;
    doc.Parse<0>((const char *const)data);
    if (doc.HasParseError()) {
        Log::Error(Event::ParseStyle, "Error parsing style JSON at %i: %s", doc.GetErrorOffset(), doc.GetParseError());
        throw error::style_parse(doc.GetErrorOffset(), doc.GetParseError());
    }

    StyleParser parser;
    parser.parse(doc);

    layers = parser.getLayers();
    sprite_url = parser.getSprite();
    glyph_url = parser.getGlyphURL();
}

}
