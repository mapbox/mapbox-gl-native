#include <mbgl/style/style_parser.hpp>
#include <mbgl/layer/fill_layer.hpp>
#include <mbgl/layer/line_layer.hpp>
#include <mbgl/layer/circle_layer.hpp>
#include <mbgl/layer/symbol_layer.hpp>
#include <mbgl/layer/raster_layer.hpp>
#include <mbgl/layer/background_layer.hpp>

#include <mbgl/platform/log.hpp>

#include <algorithm>

namespace mbgl {

StyleParser::~StyleParser() = default;

void StyleParser::parse(const JSVal& document) {
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
        const JSVal& sprite = document["sprite"];
        if (sprite.IsString()) {
            spriteURL = { sprite.GetString(), sprite.GetStringLength() };
        }
    }

    if (document.HasMember("glyphs")) {
        const JSVal& glyphs = document["glyphs"];
        if (glyphs.IsString()) {
            glyphURL = { glyphs.GetString(), glyphs.GetStringLength() };
        }
    }
}

void StyleParser::parseSources(const JSVal& value) {
    if (!value.IsObject()) {
        Log::Warning(Event::ParseStyle, "sources must be an object");
        return;
    }

    rapidjson::Value::ConstMemberIterator itr = value.MemberBegin();
    for (; itr != value.MemberEnd(); ++itr) {
        const JSVal& nameVal = itr->name;
        const JSVal& sourceVal = itr->value;

        std::unique_ptr<Source> source = std::make_unique<Source>();

        source->info.source_id = { nameVal.GetString(), nameVal.GetStringLength() };

        if (!sourceVal.HasMember("type")) {
            Log::Warning(Event::ParseStyle, "source must have a type");
            continue;
        }

        const JSVal& typeVal = sourceVal["type"];
        if (!typeVal.IsString()) {
            Log::Warning(Event::ParseStyle, "source type must have one of the enum values");
            continue;
        }

        source->info.type = SourceTypeClass({ typeVal.GetString(), typeVal.GetStringLength() });

        switch (source->info.type) {
        case SourceType::Vector:
            if (!parseVectorSource(*source, sourceVal)) {
                continue;
            }
            break;
        case SourceType::Raster:
            if (!parseRasterSource(*source, sourceVal)) {
                continue;
            }
            break;
        default:
            Log::Warning(Event::ParseStyle, "source type %s is not supported", SourceTypeClass(source->info.type).c_str());
        }

        sourcesMap.emplace(source->info.source_id, source.get());
        sources.emplace_back(std::move(source));
    }
}

bool StyleParser::parseVectorSource(Source& source, const JSVal& sourceVal) {
    // A vector tile source either specifies the URL of a TileJSON file...
    if (sourceVal.HasMember("url")) {
        const JSVal& urlVal = sourceVal["url"];

        if (!urlVal.IsString()) {
            Log::Warning(Event::ParseStyle, "source url must be a string");
            return false;
        }

        source.info.url = { urlVal.GetString(), urlVal.GetStringLength() };

    } else {
        // ...or the TileJSON directly.
        source.info.parseTileJSONProperties(sourceVal);
    }

    return true;
}

bool StyleParser::parseRasterSource(Source& source, const JSVal& sourceVal) {
    if (sourceVal.HasMember("tileSize")) {
        const JSVal& tileSizeVal = sourceVal["tileSize"];

        if (!tileSizeVal.IsUint()) {
            Log::Warning(Event::ParseStyle, "source tileSize must be an unsigned integer");
            return false;
        }

        unsigned int intValue = tileSizeVal.GetUint();
        if (intValue > std::numeric_limits<uint16_t>::max()) {
            Log::Warning(Event::ParseStyle, "values for tileSize that are larger than %d are not supported", std::numeric_limits<uint16_t>::max());
            return false;
        }

        source.info.tile_size = intValue;
    }

    // A raster tile source either specifies the URL of a TileJSON file...
    if (sourceVal.HasMember("url")) {
        const JSVal& urlVal = sourceVal["url"];

        if (!urlVal.IsString()) {
            Log::Warning(Event::ParseStyle, "source url must be a string");
            return false;
        }

        source.info.url = { urlVal.GetString(), urlVal.GetStringLength() };

    } else {
        // ...or the TileJSON directly.
        source.info.parseTileJSONProperties(sourceVal);
    }

    return true;
}

void StyleParser::parseLayers(const JSVal& value) {
    std::vector<std::string> ids;

    if (!value.IsArray()) {
        Log::Warning(Event::ParseStyle, "layers must be an array");
        return;
    }

    for (rapidjson::SizeType i = 0; i < value.Size(); ++i) {
        const JSVal& layerValue = value[i];

        if (!layerValue.IsObject()) {
            Log::Warning(Event::ParseStyle, "layer must be an object");
            continue;
        }

        if (!layerValue.HasMember("id")) {
            Log::Warning(Event::ParseStyle, "layer must have an id");
            continue;
        }

        const JSVal& id = layerValue["id"];
        if (!id.IsString()) {
            Log::Warning(Event::ParseStyle, "layer id must be a string");
            continue;
        }

        const std::string layerID = { id.GetString(), id.GetStringLength() };
        if (layersMap.find(layerID) != layersMap.end()) {
            Log::Warning(Event::ParseStyle, "duplicate layer id %s", layerID.c_str());
            continue;
        }

        layersMap.emplace(layerID, std::pair<const JSVal&, std::unique_ptr<StyleLayer>> { layerValue, nullptr });
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

void StyleParser::parseLayer(const std::string& id, const JSVal& value, std::unique_ptr<StyleLayer>& layer) {
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
        const JSVal& refVal = value["ref"];
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

        StyleLayer* reference = it->second.second.get();
        if (!reference) {
            return;
        }

        layer = reference->clone();
        layer->id = id;
        layer->ref = ref;

    } else {
        // Otherwise, parse the source/source-layer/filter/render keys to form the bucket.
        if (!value.HasMember("type")) {
            Log::Warning(Event::ParseStyle, "layer '%s' is missing a type", id.c_str());
            return;
        }

        const JSVal& typeVal = value["type"];
        if (!typeVal.IsString()) {
            Log::Warning(Event::ParseStyle, "layer '%s' has an invalid type", id.c_str());
            return;
        }

        std::string type { typeVal.GetString(), typeVal.GetStringLength() };

        if (type == "fill") {
            layer = std::make_unique<FillLayer>();
        } else if (type == "line") {
            layer = std::make_unique<LineLayer>();
        } else if (type == "circle") {
            layer = std::make_unique<CircleLayer>();
        } else if (type == "symbol") {
            layer = std::make_unique<SymbolLayer>();
        } else if (type == "raster") {
            layer = std::make_unique<RasterLayer>();
        } else if (type == "background") {
            layer = std::make_unique<BackgroundLayer>();
        } else {
            Log::Warning(Event::ParseStyle, "unknown type '%s' for layer '%s'", type.c_str(), id.c_str());
            return;
        }

        layer->id = id;

        if (value.HasMember("source")) {
            const JSVal& value_source = value["source"];
            if (value_source.IsString()) {
                layer->source = { value_source.GetString(), value_source.GetStringLength() };
                auto source_it = sourcesMap.find(layer->source);
                if (source_it == sourcesMap.end()) {
                    Log::Warning(Event::ParseStyle, "can't find source '%s' required for layer '%s'", layer->source.c_str(), layer->id.c_str());
                }
            } else {
                Log::Warning(Event::ParseStyle, "source of layer '%s' must be a string", layer->id.c_str());
            }
        }

        if (value.HasMember("source-layer")) {
            const JSVal& value_source_layer = value["source-layer"];
            if (value_source_layer.IsString()) {
                layer->sourceLayer = { value_source_layer.GetString(), value_source_layer.GetStringLength() };
            } else {
                Log::Warning(Event::ParseStyle, "source-layer of layer '%s' must be a string", layer->id.c_str());
            }
        }

        if (value.HasMember("filter")) {
            layer->filter = parseFilterExpression(value["filter"]);
        }

        if (value.HasMember("minzoom")) {
            const JSVal& min_zoom = value["minzoom"];
            if (min_zoom.IsNumber()) {
                layer->minZoom = min_zoom.GetDouble();
            } else {
                Log::Warning(Event::ParseStyle, "minzoom of layer %s must be numeric", layer->id.c_str());
            }
        }

        if (value.HasMember("maxzoom")) {
            const JSVal& max_zoom = value["maxzoom"];
            if (max_zoom.IsNumber()) {
                layer->maxZoom = max_zoom.GetDouble();
            } else {
                Log::Warning(Event::ParseStyle, "maxzoom of layer %s must be numeric", layer->id.c_str());
            }
        }

        if (value.HasMember("layout")) {
            parseVisibility(*layer, value["layout"]);
            layer->parseLayout(value["layout"]);
        }
    }

    layer->parsePaints(value);
}

void StyleParser::parseVisibility(StyleLayer& layer, const JSVal& value) {
    if (!value.HasMember("visibility")) {
        return;
    } else if (!value["visibility"].IsString()) {
        Log::Warning(Event::ParseStyle, "value of 'visibility' must be a string");
        layer.visibility = VisibilityType::Visible;
        return;
    }
    layer.visibility = VisibilityTypeClass({ value["visibility"].GetString(), value["visibility"].GetStringLength() });
}

} // namespace mbgl
