#include <mbgl/style/style_parser.hpp>
#include <mbgl/layer/fill_layer.hpp>
#include <mbgl/layer/line_layer.hpp>
#include <mbgl/layer/circle_layer.hpp>
#include <mbgl/layer/symbol_layer.hpp>
#include <mbgl/layer/raster_layer.hpp>
#include <mbgl/layer/background_layer.hpp>

#include <mbgl/platform/log.hpp>

#include <mapbox/geojsonvt.hpp>
#include <mapbox/geojsonvt/convert.hpp>

#include <mbgl/util/mapbox.hpp>

#include <rapidjson/document.h>
#include <rapidjson/error/en.h>

#include <algorithm>
#include <sstream>
#include <set>

namespace mbgl {


namespace {

void parseTileJSONMember(const JSValue& value, std::vector<std::string>& target, const char* name) {
    if (!value.HasMember(name)) {
        return;
    }

    const JSValue& property = value[name];
    if (!property.IsArray()) {
        return;
    }

    for (rapidjson::SizeType i = 0; i < property.Size(); i++) {
        if (!property[i].IsString()) {
            return;
        }
    }

    for (rapidjson::SizeType i = 0; i < property.Size(); i++) {
        target.emplace_back(std::string(property[i].GetString(), property[i].GetStringLength()));
    }
}

void parseTileJSONMember(const JSValue& value, std::string& target, const char* name) {
    if (!value.HasMember(name)) {
        return;
    }

    const JSValue& property = value[name];
    if (!property.IsString()) {
        return;
    }

    target = { property.GetString(), property.GetStringLength() };
}

void parseTileJSONMember(const JSValue& value, uint8_t& target, const char* name) {
    if (!value.HasMember(name)) {
        return;
    }

    const JSValue& property = value[name];
    if (!property.IsUint()) {
        return;
    }

    unsigned int uint = property.GetUint();
    if (uint > std::numeric_limits<uint8_t>::max()) {
        return;
    }

    target = uint;
}

void parseTileJSONMember(const JSValue& value, std::array<double, 4>& target, const char* name) {
    if (!value.HasMember(name)) {
        return;
    }

    const JSValue& property = value[name];
    if (!property.IsArray() || property.Size() > 4) {
        return;
    }

    for (rapidjson::SizeType i = 0; i < property.Size(); i++) {
        if (!property[i].IsNumber()) {
            return;
        }
    }

    for (rapidjson::SizeType i = 0; i < property.Size(); i++) {
        target[i] = property[i].GetDouble();
    }
}

} // end namespace

StyleParser::~StyleParser() = default;

void StyleParser::parse(const std::string& json) {
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

void StyleParser::parseSources(const JSValue& value) {
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
            Log::Warning(Event::ParseStyle, "source type must have one of the enum values");
            continue;
        }

        const auto type = SourceTypeClass({ typeVal.GetString(), typeVal.GetStringLength() });

        // Sources can have URLs, either because they reference an external TileJSON file, or
        // because reference a GeoJSON file. They don't have to have one though when all source
        // parameters are specified inline.
        std::string url;

        uint16_t tileSize = util::tileSize;

        std::unique_ptr<SourceInfo> info;
        std::unique_ptr<mapbox::geojsonvt::GeoJSONVT> geojsonvt;

        switch (type) {
        case SourceType::Raster:
            if (sourceVal.HasMember("tileSize")) {
                const JSValue& tileSizeVal = sourceVal["tileSize"];
                if (tileSizeVal.IsNumber() && tileSizeVal.GetUint64() <= std::numeric_limits<uint16_t>::max()) {
                    tileSize = tileSizeVal.GetUint64();
                } else {
                    Log::Error(Event::ParseStyle, "invalid tileSize");
                    continue;
                }
            }
            // Fall through. Vector sources are forbidden from having a tileSize.

        case SourceType::Vector:
            if (sourceVal.HasMember("url")) {
                const JSValue& urlVal = sourceVal["url"];
                if (urlVal.IsString()) {
                    url = { urlVal.GetString(), urlVal.GetStringLength() };
                } else {
                    Log::Error(Event::ParseStyle, "source url must be a string");
                    continue;
                }
            } else {
                info = parseTileJSON(sourceVal);
            }
            break;

        case SourceType::GeoJSON:
            info = std::make_unique<SourceInfo>();

            // We should probably split this up to have URLs in the url property, and actual data
            // in the data property. Until then, we're going to detect the content based on the
            // object type.
            if (sourceVal.HasMember("data")) {
                const JSValue& dataVal = sourceVal["data"];
                if (dataVal.IsString()) {
                    // We need to load an external GeoJSON file
                    url = { dataVal.GetString(), dataVal.GetStringLength() };
                } else if (dataVal.IsObject()) {
                    // We need to parse dataVal as a GeoJSON object
                    geojsonvt = parseGeoJSON(dataVal);
                    info->maxZoom = geojsonvt->options.maxZoom;
                } else {
                    Log::Error(Event::ParseStyle, "GeoJSON data must be a URL or an object");
                    continue;
                }
            } else {
                Log::Error(Event::ParseStyle, "GeoJSON source must have a data value");
                continue;
            }

            break;

        default:
            Log::Error(Event::ParseStyle, "source type '%s' is not supported", typeVal.GetString());
            continue;
        }

        const std::string id { nameVal.GetString(), nameVal.GetStringLength() };
        std::unique_ptr<Source> source = std::make_unique<Source>(type, id, url, tileSize, std::move(info), std::move(geojsonvt));

        sourcesMap.emplace(id, source.get());
        sources.emplace_back(std::move(source));
    }
}

std::unique_ptr<mapbox::geojsonvt::GeoJSONVT> StyleParser::parseGeoJSON(const JSValue& value) {
    using namespace mapbox::geojsonvt;

    Options options;
    options.buffer = util::EXTENT / util::tileSize * 128;
    options.extent = util::EXTENT;

    try {
        return std::make_unique<GeoJSONVT>(Convert::convert(value, 0), options);
    } catch (const std::exception& ex) {
        Log::Error(Event::ParseStyle, "Failed to parse GeoJSON data: %s", ex.what());
        // Create an empty GeoJSON VT object to make sure we're not infinitely waiting for
        // tiles to load.
        return std::make_unique<GeoJSONVT>(std::vector<ProjectedFeature>{}, options);
    }
}

std::unique_ptr<SourceInfo> StyleParser::parseTileJSON(const std::string& json, const std::string& sourceURL, SourceType type, uint16_t tileSize) {
    rapidjson::GenericDocument<rapidjson::UTF8<>, rapidjson::CrtAllocator> document;
    document.Parse<0>(json.c_str());

    if (document.HasParseError()) {
        std::stringstream message;
        message << document.GetErrorOffset() << " - " << rapidjson::GetParseError_En(document.GetParseError());
        throw std::runtime_error(message.str());
    }

    std::unique_ptr<SourceInfo> result = StyleParser::parseTileJSON(document);

    // TODO: Remove this hack by delivering proper URLs in the TileJSON to begin with.
    if (util::mapbox::isMapboxURL(sourceURL)) {
        for (auto& url : result->tiles) {
            url = util::mapbox::canonicalizeTileURL(url, type, tileSize);
        }
    }

    return result;
}

std::unique_ptr<SourceInfo> StyleParser::parseTileJSON(const JSValue& value) {
    auto info = std::make_unique<SourceInfo>();
    parseTileJSONMember(value, info->tiles, "tiles");
    parseTileJSONMember(value, info->minZoom, "minzoom");
    parseTileJSONMember(value, info->maxZoom, "maxzoom");
    parseTileJSONMember(value, info->attribution, "attribution");

    std::array<double, 4> array;
    parseTileJSONMember(value, array, "center");
    info->center = { array[0], array[1] };
    info->zoom = array[2];
    parseTileJSONMember(value, array, "bounds");
    info->bounds = LatLngBounds::hull({ array[0], array[1] }, { array[2], array[3] });

    return info;
}

void StyleParser::parseLayers(const JSValue& value) {
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

        layersMap.emplace(layerID, std::pair<const JSValue&, std::unique_ptr<StyleLayer>> { layerValue, nullptr });
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

void StyleParser::parseLayer(const std::string& id, const JSValue& value, std::unique_ptr<StyleLayer>& layer) {
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

        const JSValue& typeVal = value["type"];
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
            const JSValue& value_source = value["source"];
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
            const JSValue& value_source_layer = value["source-layer"];
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
            const JSValue& min_zoom = value["minzoom"];
            if (min_zoom.IsNumber()) {
                layer->minZoom = min_zoom.GetDouble();
            } else {
                Log::Warning(Event::ParseStyle, "minzoom of layer %s must be numeric", layer->id.c_str());
            }
        }

        if (value.HasMember("maxzoom")) {
            const JSValue& max_zoom = value["maxzoom"];
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

void StyleParser::parseVisibility(StyleLayer& layer, const JSValue& value) {
    if (!value.HasMember("visibility")) {
        return;
    } else if (!value["visibility"].IsString()) {
        Log::Warning(Event::ParseStyle, "value of 'visibility' must be a string");
        layer.visibility = VisibilityType::Visible;
        return;
    }
    layer.visibility = VisibilityTypeClass({ value["visibility"].GetString(), value["visibility"].GetStringLength() });
}

std::vector<std::string> StyleParser::fontStacks() const {
    std::set<std::string> result;

    for (const auto& layer : layers) {
        if (layer->is<SymbolLayer>()) {
            LayoutProperty<std::string> property = layer->as<SymbolLayer>()->layout.textFont;
            if (property.parsedValue) {
                for (const auto& stop : property.parsedValue->getStops()) {
                    result.insert(stop.second);
                }
            } else {
                result.insert(property.value);
            }
        }
    }

    return std::vector<std::string>(result.begin(), result.end());
}

} // namespace mbgl
