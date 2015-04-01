#include <mbgl/style/style.hpp>
#include <mbgl/map/sprite.hpp>
#include <mbgl/style/style_layer.hpp>
#include <mbgl/style/style_parser.hpp>
#include <mbgl/style/style_bucket.hpp>
#include <mbgl/util/constants.hpp>
#include <mbgl/util/error.hpp>
#include <mbgl/util/std.hpp>
#include <mbgl/util/uv_detail.hpp>
#include <mbgl/platform/log.hpp>
#include <csscolorparser/csscolorparser.hpp>

#include <rapidjson/document.h>

#include <algorithm>

namespace mbgl {

Style::Style()
    : mtx(util::make_unique<uv::rwlock>()) {
}

// Note: This constructor is seemingly empty, but we need to declare it anyway
// because this file includes uv_detail.hpp, which has the declarations necessary
// for deleting the std::unique_ptr<uv::rwlock>.
Style::~Style() {}

void Style::updateProperties(float z, std::chrono::steady_clock::time_point now) {
    uv::writelock lock(mtx);

    zoomHistory.update(z, now);

    for (const auto& layer : layers) {
        layer->updateProperties(z, now, zoomHistory);
    }
}

const std::string &Style::getSpriteURL() const {
    return sprite_url;
}

void Style::setDefaultTransitionDuration(std::chrono::steady_clock::duration duration) {
    defaultTransition.duration = duration;
}

void Style::cascadeClasses(const std::vector<std::string>& classes) {
    std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
    for (const auto& layer : layers) {
        layer->setClasses(classes, now, defaultTransition);
    }
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
        throw error::style_parse(doc.GetErrorOffset(), doc.GetParseError());
    }

    StyleParser parser;
    parser.parse(doc);

    layers = parser.getLayers();
    sprite_url = parser.getSprite();
    glyph_url = parser.getGlyphURL();
}

}
