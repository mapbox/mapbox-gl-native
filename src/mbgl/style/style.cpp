#include <mbgl/style/style.hpp>
#include <mbgl/map/sprite.hpp>
#include <mbgl/map/source.hpp>
#include <mbgl/style/style_layer.hpp>
#include <mbgl/style/style_parser.hpp>
#include <mbgl/style/style_bucket.hpp>
#include <mbgl/util/constants.hpp>
#include <mbgl/util/uv_detail.hpp>
#include <mbgl/platform/log.hpp>
#include <csscolorparser/csscolorparser.hpp>

#include <rapidjson/document.h>

#include <algorithm>

namespace mbgl {

Style::Style()
    : mtx(std::make_unique<uv::rwlock>()),
      workers(4) {
}

// Note: This constructor is seemingly empty, but we need to declare it anyway
// because this file includes uv_detail.hpp, which has the declarations necessary
// for deleting the std::unique_ptr<uv::rwlock>.
Style::~Style() {}

void Style::cascade(const std::vector<std::string>& classes) {
    TimePoint now = Clock::now();

    for (const auto& layer : layers) {
        layer->setClasses(classes, now, defaultTransition);
    }
}

void Style::recalculate(float z, TimePoint now) {
    uv::writelock lock(mtx);

    for (const auto& source : sources) {
        source->enabled = false;
    }

    zoomHistory.update(z, now);

    for (const auto& layer : layers) {
        layer->updateProperties(z, now, zoomHistory);
        if (layer->bucket && layer->bucket->source) {
            layer->bucket->source->enabled = true;
        }
    }
}

const std::string &Style::getSpriteURL() const {
    return sprite_url;
}

void Style::setDefaultTransitionDuration(Duration duration) {
    defaultTransition.duration = duration;
}

bool Style::hasTransitions() const {
    for (const auto& layer : layers) {
        if (layer->hasTransitions()) {
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
        return;
    }

    StyleParser parser;
    parser.parse(doc);

    sources = parser.getSources();
    layers = parser.getLayers();
    sprite_url = parser.getSprite();
    glyph_url = parser.getGlyphURL();
    loaded = true;
}

bool Style::isLoaded() const {
    // TODO: move loading into Style
    if (!loaded) {
        return false;
    }

    for (const auto& source : sources) {
        if (!source->isLoaded()) {
            return false;
        }
    }

    // TODO: move sprite into Style
//    if (sprite && !sprite.isLoaded()) {
//        return false;
//    }

    return true;
}

}
