#include <mbgl/style/style_parser.hpp>
#include <mbgl/style/style_layer.hpp>

#include <mbgl/platform/log.hpp>

#include <algorithm>

namespace mbgl {

void StyleParser::parse(const JSVal& document) {
    if (document.HasMember("version")) {
        version = document["version"].GetInt();
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
        parseSprite(document["sprite"]);
    }

    if (document.HasMember("glyphs")) {
        parseGlyphURL(document["glyphs"]);
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

        if (sourceVal.HasMember("url")) {
            const JSVal& urlVal = sourceVal["url"];

            if (!urlVal.IsString()) {
                Log::Warning(Event::ParseStyle, "source url must be a string");
                continue;
            }

            source->info.url = { urlVal.GetString(), urlVal.GetStringLength() };
        }

        if (sourceVal.HasMember("tileSize")) {
            const JSVal& tileSizeVal = sourceVal["tileSize"];

            if (!tileSizeVal.IsUint()) {
                Log::Warning(Event::ParseStyle, "source tileSize must be an unsigned integer");
                continue;
            }

            unsigned int intValue = tileSizeVal.GetUint();
            if (intValue > std::numeric_limits<uint16_t>::max()) {
                Log::Warning(Event::ParseStyle, "values for tileSize that are larger than %d are not supported", std::numeric_limits<uint16_t>::max());
                continue;
            }

            source->info.tile_size = intValue;
        }

        source->info.parseTileJSONProperties(sourceVal);

        sourcesMap.emplace(source->info.source_id, source.get());
        sources.emplace_back(std::move(source));
    }
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

        layersMap.emplace(layerID, std::pair<const JSVal&, util::ptr<StyleLayer>> { layerValue, nullptr });
        ids.push_back(layerID);
    }

    for (const auto& id : ids) {
        auto it = layersMap.find(id);

        parseLayer(it->first,
                   it->second.first,
                   it->second.second);

        if (it->second.second) {
            layers.emplace_back(it->second.second);
        }
    }
}

void StyleParser::parseLayer(const std::string& id, const JSVal& value, util::ptr<StyleLayer>& layer) {
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

        util::ptr<StyleLayer> reference = it->second.second;
        if (!reference) {
            return;
        }

        layer = StyleLayer::create(reference->type);
        layer->id = id;
        layer->type = reference->type;
        layer->bucket = reference->bucket;

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
        StyleLayerType typeClass = StyleLayerTypeClass(type);
        layer = StyleLayer::create(typeClass);

        if (!layer) {
            Log::Warning(Event::ParseStyle, "unknown type '%s' for layer '%s'", type.c_str(), id.c_str());
            return;
        }

        layer->id = id;
        layer->type = typeClass;
        layer->bucket = std::make_shared<StyleBucket>(layer->type);

        // We name the buckets according to the layer that defined it.
        layer->bucket->name = layer->id;

        if (value.HasMember("source")) {
            const JSVal& value_source = value["source"];
            if (value_source.IsString()) {
                layer->bucket->source = { value_source.GetString(), value_source.GetStringLength() };
                auto source_it = sourcesMap.find(layer->bucket->source);
                if (source_it == sourcesMap.end()) {
                    Log::Warning(Event::ParseStyle, "can't find source '%s' required for layer '%s'", layer->bucket->source.c_str(), layer->id.c_str());
                }
            } else {
                Log::Warning(Event::ParseStyle, "source of layer '%s' must be a string", layer->id.c_str());
            }
        }

        if (value.HasMember("source-layer")) {
            const JSVal& value_source_layer = value["source-layer"];
            if (value_source_layer.IsString()) {
                layer->bucket->source_layer = { value_source_layer.GetString(), value_source_layer.GetStringLength() };
            } else {
                Log::Warning(Event::ParseStyle, "source-layer of layer '%s' must be a string", layer->id.c_str());
            }
        }

        if (value.HasMember("filter")) {
            layer->bucket->filter = parseFilterExpression(value["filter"]);
        }

        if (value.HasMember("minzoom")) {
            const JSVal& min_zoom = value["minzoom"];
            if (min_zoom.IsNumber()) {
                layer->bucket->min_zoom = min_zoom.GetDouble();
            } else {
                Log::Warning(Event::ParseStyle, "minzoom of layer %s must be numeric", layer->id.c_str());
            }
        }

        if (value.HasMember("maxzoom")) {
            const JSVal& max_zoom = value["maxzoom"];
            if (max_zoom.IsNumber()) {
                layer->bucket->max_zoom = max_zoom.GetDouble();
            } else {
                Log::Warning(Event::ParseStyle, "maxzoom of layer %s must be numeric", layer->id.c_str());
            }
        }

        if (value.HasMember("layout")) {
            parseVisibility(*layer->bucket, value["layout"]);
            layer->parseLayout(value["layout"]);
        }
    }

    layer->parsePaints(value);
}

void StyleParser::parseSprite(const JSVal& value) {
    if (value.IsString()) {
        sprite = { value.GetString(), value.GetStringLength() };
    }
}

void StyleParser::parseGlyphURL(const JSVal& value) {
    if (value.IsString()) {
        glyph_url = { value.GetString(), value.GetStringLength() };
    }
}

void StyleParser::parseVisibility(StyleBucket& bucket, const JSVal& value) {
    if (!value.HasMember("visibility")) {
        return;
    } else if (!value["visibility"].IsString()) {
        Log::Warning(Event::ParseStyle, "value of 'visibility' must be a string");
        bucket.visibility = VisibilityType::Visible;
        return;
    }
    bucket.visibility = VisibilityTypeClass({ value["visibility"].GetString(), value["visibility"].GetStringLength() });
}

}
