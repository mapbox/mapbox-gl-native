#include <mbgl/style/style.hpp>
#include <mbgl/map/sprite.hpp>
#include <mbgl/style/style_layer_group.hpp>
#include <mbgl/style/style_parser.hpp>
#include <mbgl/style/style_bucket.hpp>
#include <mbgl/util/constants.hpp>
#include <mbgl/util/time.hpp>
#include <mbgl/util/error.hpp>
#include <mbgl/util/std.hpp>
#include <mbgl/util/uv_detail.hpp>
#include <csscolorparser/csscolorparser.hpp>

#include <rapidjson/document.h>


namespace mbgl {

Style::Style()
    : mtx(std::make_unique<uv::rwlock>()) {
}

// Note: This constructor is seemingly empty, but we need to declare it anyway
// because this file includes uv_detail.hpp, which has the declarations necessary
// for deleting the std::unique_ptr<uv::rwlock>.
Style::~Style() {}

void Style::updateProperties(float z, timestamp now) {
    uv::writelock lock(mtx);

    if (layers) {
        layers->updateProperties(z, now);
    }

    // Apply transitions after the first time.
    if (!initial_render_complete) {
        initial_render_complete = true;
        return;
    }
}

const std::string &Style::getSpriteURL() const {
    return sprite_url;
}

void Style::setDefaultTransitionDuration(uint16_t duration_milliseconds) {
    defaultTransition.duration = duration_milliseconds;
}

const std::vector<std::string> &Style::getAppliedClasses() const {
    return appliedClasses;
}

void Style::setAppliedClasses(const std::vector<std::string> &class_names) {
    appliedClasses = class_names;
    updateClasses();
}

void Style::toggleClass(const std::string &name) {
    if (name.length()) {
        auto it = std::find(appliedClasses.begin(), appliedClasses.end(), name);
        if (it == appliedClasses.end()) {
            appliedClasses.push_back(name);
        } else {
            appliedClasses.erase(it);
        }
    }

    updateClasses();
}

void Style::updateClasses() {
    if (layers) {
        layers->setClasses(appliedClasses, util::now(), defaultTransition);
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
        throw error::style_parse(doc.GetErrorOffset(), doc.GetParseError());
    }

    StyleParser parser;
    parser.parse(const_cast<const rapidjson::Document &>(doc));

    layers = parser.getLayers();
    sprite_url = parser.getSprite();
    glyph_url = parser.getGlyphURL();

    updateClasses();
}

const BackgroundProperties &Style::getBackgroundProperties() const {
    if (layers && layers->layers.size()) {
        const auto first = layers->layers.front();
        if (first && first->type == StyleLayerType::Background) {
            return first->getProperties<BackgroundProperties>();
        }
    }

    return defaultStyleProperties<BackgroundProperties>();
}

}
