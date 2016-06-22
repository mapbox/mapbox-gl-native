#include <mbgl/style/parser.hpp>
#include <mbgl/style/sources/raster_source_impl.hpp>
#include <mbgl/style/sources/vector_source_impl.hpp>
#include <mbgl/style/sources/geojson_source_impl.hpp>
#include <mbgl/style/layer_impl.hpp>
#include <mbgl/style/layers/fill_layer.hpp>
#include <mbgl/style/layers/line_layer.hpp>
#include <mbgl/style/layers/circle_layer.hpp>
#include <mbgl/style/layers/symbol_layer.hpp>
#include <mbgl/style/layers/raster_layer.hpp>
#include <mbgl/style/layers/background_layer.hpp>
#include <mbgl/style/rapidjson_conversion.hpp>
#include <mbgl/style/conversion.hpp>
#include <mbgl/style/conversion/layer.hpp>

#include <mbgl/platform/log.hpp>

#include <mbgl/tile/geometry_tile_data.hpp>
#include <mbgl/util/enum.hpp>
#include <mbgl/util/tileset.hpp>

#include <rapidjson/document.h>
#include <rapidjson/error/en.h>

#include <algorithm>
#include <set>

namespace mbgl {
namespace style {

Parser::~Parser() = default;

void Parser::parse(const std::string& json) {
    rapidjson::GenericDocument<rapidjson::UTF8<>, rapidjson::CrtAllocator> document;
    document.Parse<0>(json.c_str());

    if (document.HasParseError()) {
        Log::Error(Event::ParseStyle, "Error parsing style JSON at %i: %s", document.GetErrorOffset(), rapidjson::GetParseError_En(document.GetParseError()));
        return;
    }

    if (document.HasMember("version")) {
        int version = document["version"].GetInt();
        if (version != 8) {
            Log::Warning(Event::ParseStyle, "current renderer implementation only supports style spec version 8; using an outdated style will cause rendering errors");
        }
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
}

void Parser::parseSources(const JSValue& value) {
    if (!value.IsObject()) {
        Log::Warning(Event::ParseStyle, "sources must be an object");
        return;
    }

    JSValue::ConstMemberIterator itr = value.MemberBegin();
    for (; itr != value.MemberEnd(); ++itr) {
        const JSValue& nameVal = itr->name;
        const JSValue& sourceVal = itr->value;

        if (!sourceVal.HasMember("type")) {
            Log::Warning(Event::ParseStyle, "source must have a type");
            continue;
        }

        const JSValue& typeVal = sourceVal["type"];
        if (!typeVal.IsString()) {
            Log::Warning(Event::ParseStyle, "source type must be a string");
            continue;
        }

        const auto type = Enum<SourceType>::toEnum({ typeVal.GetString(), typeVal.GetStringLength() });
        if (!type) {
            Log::Warning(Event::ParseStyle, "source type must have one of the enum values");
            continue;
        }

        const std::string id { nameVal.GetString(), nameVal.GetStringLength() };
        std::unique_ptr<Source> source;

        switch (*type) {
        case SourceType::Raster: {
            source = RasterSource::Impl::parse(id, sourceVal);
            break;
        }

        case SourceType::Vector:
            source = VectorSource::Impl::parse(id, sourceVal);
            break;

        case SourceType::GeoJSON:
            source = GeoJSONSource::Impl::parse(id, sourceVal);
            break;

        default:
            Log::Error(Event::ParseStyle, "source type '%s' is not supported", typeVal.GetString());
            continue;
        }

        if (source) {
            sourcesMap.emplace(id, source.get());
            sources.emplace_back(std::move(source));
        }
    }
}

void Parser::parseLayers(const JSValue& value) {
    std::vector<std::string> ids;

    if (!value.IsArray()) {
        Log::Warning(Event::ParseStyle, "layers must be an array");
        return;
    }

    for (rapidjson::SizeType i = 0; i < value.Size(); ++i) {
        const JSValue& layerValue = value[i];

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

        layer = reference->baseImpl->cloneRef(id);
        conversion::setPaintProperties(*layer, value);
    } else {
        conversion::Result<std::unique_ptr<Layer>> converted = conversion::convert<std::unique_ptr<Layer>>(value);
        if (!converted) {
            Log::Warning(Event::ParseStyle, converted.error().message);
            return;
        }
        layer = std::move(*converted);
    }
}

std::vector<FontStack> Parser::fontStacks() const {
    std::set<FontStack> result;

    for (const auto& layer : layers) {
        if (layer->is<SymbolLayer>()) {
            PropertyValue<FontStack> textFont = layer->as<SymbolLayer>()->getTextFont();
            if (textFont.isUndefined()) {
                result.insert({"Open Sans Regular", "Arial Unicode MS Regular"});
            } else if (textFont.isConstant()) {
                result.insert(textFont.asConstant());
            } else if (textFont.isFunction()) {
                for (const auto& stop : textFont.asFunction().getStops()) {
                    result.insert(stop.second);
                }
            }
        }
    }

    return std::vector<FontStack>(result.begin(), result.end());
}

} // namespace style
} // namespace mbgl
