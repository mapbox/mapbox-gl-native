#include <mbgl/style/parser.hpp>
#include <mbgl/style/layer_impl.hpp>
#include <mbgl/style/layers/symbol_layer.hpp>
#include <mbgl/style/rapidjson_conversion.hpp>
#include <mbgl/style/conversion.hpp>
#include <mbgl/style/conversion/coordinate.hpp>
#include <mbgl/style/conversion/source.hpp>
#include <mbgl/style/conversion/layer.hpp>
#include <mbgl/style/conversion/light.hpp>
#include <mbgl/style/conversion/transition_options.hpp>

#include <mbgl/util/logging.hpp>
#include <mbgl/util/string.hpp>

#include <mapbox/geojsonvt.hpp>

#include <rapidjson/document.h>
#include <rapidjson/error/en.h>

#include <algorithm>
#include <set>
#include <sstream>

namespace mbgl {
namespace style {

Parser::~Parser() = default;

StyleParseResult Parser::parse(const std::string& json) {
    rapidjson::GenericDocument<rapidjson::UTF8<>, rapidjson::CrtAllocator> document;
    document.Parse<0>(json.c_str());

    if (document.HasParseError()) {
        std::stringstream message;
        message <<  document.GetErrorOffset() << " - "
            << rapidjson::GetParseError_En(document.GetParseError());

        return std::make_exception_ptr(std::runtime_error(message.str()));
    }

    if (!document.IsObject()) {
        return std::make_exception_ptr(std::runtime_error("style must be an object"));
    }

    if (document.HasMember("version")) {
        const JSValue& versionValue = document["version"];
        const int version = versionValue.IsNumber() ? versionValue.GetInt() : 0;
        if (version != 8) {
            Log::Warning(Event::ParseStyle, "current renderer implementation only supports style spec version 8; using an outdated style will cause rendering errors");
        }
    }

    if (document.HasMember("name")) {
        const JSValue& value = document["name"];
        if (value.IsString()) {
            name = { value.GetString(), value.GetStringLength() };
        }
    }

    if (document.HasMember("center")) {
        const JSValue& value = document["center"];
        conversion::Error error;
        auto convertedLatLng = conversion::convert<LatLng>(value, error);
        if (convertedLatLng) {
            latLng = *convertedLatLng;
        } else {
            Log::Warning(Event::ParseStyle, "center coordinate must be a longitude, latitude pair");
        }
    }

    if (document.HasMember("zoom")) {
        const JSValue& value = document["zoom"];
        if (value.IsNumber()) {
            zoom = value.GetDouble();
        }
    }

    if (document.HasMember("bearing")) {
        const JSValue& value = document["bearing"];
        if (value.IsNumber()) {
            bearing = value.GetDouble();
        }
    }

    if (document.HasMember("pitch")) {
        const JSValue& value = document["pitch"];
        if (value.IsNumber()) {
            pitch = value.GetDouble();
        }
    }

    if (document.HasMember("transition")) {
        parseTransition(document["transition"]);
    }

    if (document.HasMember("light")) {
        parseLight(document["light"]);
    }

    if (document.HasMember("sources")) {
        parseSources(document["sources"]);
    }

    if (document.HasMember("layers")) {
        parseLayers(document["layers"]);
    }

    if (document.HasMember("sprite")) {
        const JSValue& sprite = document["sprite"];
        if (sprite.IsString()) {
            spriteURL = { sprite.GetString(), sprite.GetStringLength() };
        }
    }

    if (document.HasMember("glyphs")) {
        const JSValue& glyphs = document["glyphs"];
        if (glyphs.IsString()) {
            glyphURL = { glyphs.GetString(), glyphs.GetStringLength() };
        }
    }

    // Call for side effect of logging warnings for invalid values.
    fontStacks();

    return nullptr;
}

void Parser::parseTransition(const JSValue& value) {
    conversion::Error error;
    optional<TransitionOptions> converted = conversion::convert<TransitionOptions>(value, error);
    if (!converted) {
        Log::Warning(Event::ParseStyle, error.message);
        return;
    }

    transition = std::move(*converted);
}

void Parser::parseLight(const JSValue& value) {
    conversion::Error error;
    optional<Light> converted = conversion::convert<Light>(value, error);
    if (!converted) {
        Log::Warning(Event::ParseStyle, error.message);
        return;
    }

    light = std::move(*converted);
}

void Parser::parseSources(const JSValue& value) {
    if (!value.IsObject()) {
        Log::Warning(Event::ParseStyle, "sources must be an object");
        return;
    }

    for (const auto& property : value.GetObject()) {
        std::string id { property.name.GetString(), property.name.GetStringLength() };

        conversion::Error error;
        optional<std::unique_ptr<Source>> source =
            conversion::convert<std::unique_ptr<Source>>(property.value, error, id);
        if (!source) {
            Log::Warning(Event::ParseStyle, error.message);
            continue;
        }

        sourcesMap.emplace(id, (*source).get());
        sources.emplace_back(std::move(*source));
    }
}

void Parser::parseLayers(const JSValue& value) {
    std::vector<std::string> ids;

    if (!value.IsArray()) {
        Log::Warning(Event::ParseStyle, "layers must be an array");
        return;
    }

    for (auto& layerValue : value.GetArray()) {
        if (!layerValue.IsObject()) {
            Log::Warning(Event::ParseStyle, "layer must be an object");
            continue;
        }

        if (!layerValue.HasMember("id")) {
            Log::Warning(Event::ParseStyle, "layer must have an id");
            continue;
        }

        const JSValue& id = layerValue["id"];
        if (!id.IsString()) {
            Log::Warning(Event::ParseStyle, "layer id must be a string");
            continue;
        }

        const std::string layerID = { id.GetString(), id.GetStringLength() };
        if (layersMap.find(layerID) != layersMap.end()) {
            Log::Warning(Event::ParseStyle, "duplicate layer id %s", layerID.c_str());
            continue;
        }

        layersMap.emplace(layerID, std::pair<const JSValue&, std::unique_ptr<Layer>> { layerValue, nullptr });
        ids.push_back(layerID);
    }

    for (const auto& id : ids) {
        auto it = layersMap.find(id);

        parseLayer(it->first,
                   it->second.first,
                   it->second.second);
    }

    for (const auto& id : ids) {
        auto it = layersMap.find(id);

        if (it->second.second) {
            layers.emplace_back(std::move(it->second.second));
        }
    }
}

void Parser::parseLayer(const std::string& id, const JSValue& value, std::unique_ptr<Layer>& layer) {
    if (layer) {
        // Skip parsing this again. We already have a valid layer definition.
        return;
    }

    // Make sure we have not previously attempted to parse this layer.
    if (std::find(stack.begin(), stack.end(), id) != stack.end()) {
        Log::Warning(Event::ParseStyle, "layer reference of '%s' is circular", id.c_str());
        return;
    }

    if (value.HasMember("ref")) {
        // This layer is referencing another layer. Recursively parse that layer.
        const JSValue& refVal = value["ref"];
        if (!refVal.IsString()) {
            Log::Warning(Event::ParseStyle, "layer ref of '%s' must be a string", id.c_str());
            return;
        }

        const std::string ref { refVal.GetString(), refVal.GetStringLength() };
        auto it = layersMap.find(ref);
        if (it == layersMap.end()) {
            Log::Warning(Event::ParseStyle, "layer '%s' references unknown layer %s", id.c_str(), ref.c_str());
            return;
        }

        // Recursively parse the referenced layer.
        stack.push_front(id);
        parseLayer(it->first,
                   it->second.first,
                   it->second.second);
        stack.pop_front();

        Layer* reference = it->second.second.get();
        if (!reference) {
            return;
        }

        layer = reference->cloneRef(id);
        conversion::setPaintProperties(*layer, conversion::Convertible(&value));
    } else {
        conversion::Error error;
        optional<std::unique_ptr<Layer>> converted = conversion::convert<std::unique_ptr<Layer>>(value, error);
        if (!converted) {
            Log::Warning(Event::ParseStyle, error.message);
            return;
        }
        layer = std::move(*converted);
    }
}

std::vector<FontStack> Parser::fontStacks() const {
    std::set<FontStack> result;

    for (const auto& layer : layers) {
        if (layer->is<SymbolLayer>() && !layer->as<SymbolLayer>()->getTextField().isUndefined()) {
            layer->as<SymbolLayer>()->getTextFont().match(
                [&] (Undefined) {
                    result.insert({"Open Sans Regular", "Arial Unicode MS Regular"});
                },
                [&] (const FontStack& constant) {
                    result.insert(constant);
                },
                [&] (const auto& function) {
                    for (const auto& value : function.possibleOutputs()) {
                        if (value) {
                            result.insert(*value);
                        } else {
                            Log::Warning(Event::ParseStyle, "Layer '%s' has an invalid value for text-font and will not work offline. Output values must be contained as literals within the expression.", layer->getID().c_str());
                            break;
                        }
                    }
                }
            );
        }
    }

    return std::vector<FontStack>(result.begin(), result.end());
}

} // namespace style
} // namespace mbgl
