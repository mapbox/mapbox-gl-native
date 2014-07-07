#include <llmr/style/style.hpp>
#include <llmr/style/style_layer_group.hpp>
#include <llmr/style/style_parser.hpp>
#include <llmr/style/style_bucket.hpp>
#include <llmr/util/constants.hpp>
#include <llmr/util/time.hpp>
#include <csscolorparser/csscolorparser.hpp>

#include <rapidjson/document.h>


namespace llmr {

Style::Style() {
}

void Style::updateSources() {
    activeSources.clear();
    updateSources(layers);
}

const std::set<std::shared_ptr<Source>> Style::getActiveSources() const {
    return activeSources;
}

void Style::updateSources(const std::shared_ptr<StyleLayerGroup> &group) {
    if (!group) {
        return;
    }
    for (const std::shared_ptr<StyleLayer> &layer : group->layers) {
        if (!layer) continue;
        if (layer->bucket) {
            if (layer->bucket->source) {
                activeSources.emplace(layer->bucket->source);
            }
        } else if (layer->layers) {
            updateSources(layer->layers);
        }
    }
}

void Style::updateProperties(float z, timestamp now) {
    uv::writelock lock(mtx);

    updateSources();

    if (layers) {
        layers->updateProperties(z, now);
    }

    // Apply transitions after the first time.
    if (!initial_render_complete) {
        initial_render_complete = true;
        return;
    }
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
        if (first && first->id == "background") {
            return first->getProperties<BackgroundProperties>();
        }
    }

    return defaultStyleProperties<BackgroundProperties>();
}

}
