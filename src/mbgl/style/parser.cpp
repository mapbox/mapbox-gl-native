#include <mbgl/style/parser.hpp>
#include <mbgl/style/layer_impl.hpp>
#include <mbgl/style/layers/fill_layer.hpp>
#include <mbgl/style/layers/line_layer.hpp>
#include <mbgl/style/layers/circle_layer.hpp>
#include <mbgl/style/layers/symbol_layer.hpp>
#include <mbgl/style/layers/raster_layer.hpp>
#include <mbgl/style/layers/background_layer.hpp>

#include <mbgl/platform/log.hpp>

#include <mapbox/geojsonvt.hpp>
#include <mapbox/geojsonvt/convert.hpp>

#include <mbgl/tile/geometry_tile.hpp>
#include <mbgl/util/mapbox.hpp>

#include <rapidjson/document.h>
#include <rapidjson/error/en.h>

#include <algorithm>
#include <sstream>
#include <set>

namespace mbgl {
namespace style {

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
            Log::Warning(Event::ParseStyle, "source type must have one of the enum values");
            continue;
        }

        const auto type = SourceTypeClass({ typeVal.GetString(), typeVal.GetStringLength() });

        // Sources can have URLs, either because they reference an external TileJSON file, or
        // because reference a GeoJSON file. They don't have to have one though when all source
        // parameters are specified inline.
        std::string url;

        uint16_t tileSize = util::tileSize;

        std::unique_ptr<Tileset> tileset;
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
                tileset = parseTileJSON(sourceVal);
            }
            break;

        case SourceType::GeoJSON:
            tileset = std::make_unique<Tileset>();

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
                    tileset->maxZoom = geojsonvt->options.maxZoom;
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
        std::unique_ptr<Source> source = std::make_unique<Source>(type, id, url, tileSize, std::move(tileset), std::move(geojsonvt));

        sourcesMap.emplace(id, source.get());
        sources.emplace_back(std::move(source));
    }
}

std::unique_ptr<mapbox::geojsonvt::GeoJSONVT> parseGeoJSON(const JSValue& value) {
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

std::unique_ptr<Tileset> parseTileJSON(const std::string& json, const std::string& sourceURL, SourceType type, uint16_t tileSize) {
    rapidjson::GenericDocument<rapidjson::UTF8<>, rapidjson::CrtAllocator> document;
    document.Parse<0>(json.c_str());

    if (document.HasParseError()) {
        std::stringstream message;
        message << document.GetErrorOffset() << " - " << rapidjson::GetParseError_En(document.GetParseError());
        throw std::runtime_error(message.str());
    }

    std::unique_ptr<Tileset> result = parseTileJSON(document);

    // TODO: Remove this hack by delivering proper URLs in the TileJSON to begin with.
    if (util::mapbox::isMapboxURL(sourceURL)) {
        for (auto& url : result->tiles) {
            url = util::mapbox::canonicalizeTileURL(url, type, tileSize);
        }
    }

    return result;
}

std::unique_ptr<Tileset> parseTileJSON(const JSValue& value) {
    auto tileset = std::make_unique<Tileset>();
    parseTileJSONMember(value, tileset->tiles, "tiles");
    parseTileJSONMember(value, tileset->minZoom, "minzoom");
    parseTileJSONMember(value, tileset->maxZoom, "maxzoom");
    parseTileJSONMember(value, tileset->attribution, "attribution");

    std::array<double, 4> array;
    parseTileJSONMember(value, array, "center");
    tileset->center = { array[0], array[1] };
    tileset->zoom = array[2];
    parseTileJSONMember(value, array, "bounds");
    tileset->bounds = LatLngBounds::hull({ array[0], array[1] }, { array[2], array[3] });

    return tileset;
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

        layer = reference->copy(id, ref);
        layer->baseImpl->parsePaints(value);
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
            layer = std::make_unique<FillLayer>(id);
        } else if (type == "line") {
            layer = std::make_unique<LineLayer>(id);
        } else if (type == "circle") {
            layer = std::make_unique<CircleLayer>(id);
        } else if (type == "symbol") {
            layer = std::make_unique<SymbolLayer>(id);
        } else if (type == "raster") {
            layer = std::make_unique<RasterLayer>(id);
        } else if (type == "background") {
            layer = std::make_unique<BackgroundLayer>(id);
        } else {
            Log::Warning(Event::ParseStyle, "unknown type '%s' for layer '%s'", type.c_str(), id.c_str());
            return;
        }

        Layer::Impl* impl = layer->baseImpl.get();

        if (value.HasMember("source")) {
            const JSValue& value_source = value["source"];
            if (value_source.IsString()) {
                impl->source = { value_source.GetString(), value_source.GetStringLength() };
                auto source_it = sourcesMap.find(impl->source);
                if (source_it == sourcesMap.end()) {
                    Log::Warning(Event::ParseStyle, "can't find source '%s' required for layer '%s'", impl->source.c_str(), impl->id.c_str());
                }
            } else {
                Log::Warning(Event::ParseStyle, "source of layer '%s' must be a string", impl->id.c_str());
            }
        }

        if (value.HasMember("source-layer")) {
            const JSValue& value_source_layer = value["source-layer"];
            if (value_source_layer.IsString()) {
                impl->sourceLayer = { value_source_layer.GetString(), value_source_layer.GetStringLength() };
            } else {
                Log::Warning(Event::ParseStyle, "source-layer of layer '%s' must be a string", impl->id.c_str());
            }
        }

        if (value.HasMember("filter")) {
            impl->filter = parseFilter(value["filter"]);
        }

        if (value.HasMember("minzoom")) {
            const JSValue& min_zoom = value["minzoom"];
            if (min_zoom.IsNumber()) {
                impl->minZoom = min_zoom.GetDouble();
            } else {
                Log::Warning(Event::ParseStyle, "minzoom of layer %s must be numeric", impl->id.c_str());
            }
        }

        if (value.HasMember("maxzoom")) {
            const JSValue& max_zoom = value["maxzoom"];
            if (max_zoom.IsNumber()) {
                impl->maxZoom = max_zoom.GetDouble();
            } else {
                Log::Warning(Event::ParseStyle, "maxzoom of layer %s must be numeric", impl->id.c_str());
            }
        }

        if (value.HasMember("layout")) {
            parseVisibility(*layer, value["layout"]);
            impl->parseLayout(value["layout"]);
        }

        impl->parsePaints(value);
    }
}

MBGL_DEFINE_ENUM_CLASS(VisibilityTypeClass, VisibilityType, {
    { VisibilityType::Visible, "visible" },
    { VisibilityType::None, "none" },
});

void Parser::parseVisibility(Layer& layer, const JSValue& value) {
    Layer::Impl& impl = *layer.baseImpl;
    if (!value.HasMember("visibility")) {
        return;
    } else if (!value["visibility"].IsString()) {
        Log::Warning(Event::ParseStyle, "value of 'visibility' must be a string");
        impl.visibility = VisibilityType::Visible;
        return;
    }
    impl.visibility = VisibilityTypeClass({ value["visibility"].GetString(), value["visibility"].GetStringLength() });
}

Value parseFeatureType(const Value& value) {
    if (value == std::string("Point")) {
        return Value(uint64_t(FeatureType::Point));
    } else if (value == std::string("LineString")) {
        return Value(uint64_t(FeatureType::LineString));
    } else if (value == std::string("Polygon")) {
        return Value(uint64_t(FeatureType::Polygon));
    } else {
        Log::Warning(Event::ParseStyle, "value for $type filter must be Point, LineString, or Polygon");
        return Value(uint64_t(FeatureType::Unknown));
    }
}

Value parseValue(const JSValue& value) {
    switch (value.GetType()) {
        case rapidjson::kNullType:
        case rapidjson::kFalseType:
            return false;

        case rapidjson::kTrueType:
            return true;

        case rapidjson::kStringType:
            return std::string { value.GetString(), value.GetStringLength() };

        case rapidjson::kNumberType:
            if (value.IsUint64()) return value.GetUint64();
            if (value.IsInt64()) return value.GetInt64();
            return value.GetDouble();

        default:
            return false;
    }
}

template <class Expression>
Filter parseUnaryFilter(const JSValue& value) {
    Filter empty;

    if (value.Size() < 2) {
        Log::Warning(Event::ParseStyle, "filter expression must have 2 elements");
        return empty;
    }

    if (!value[1u].IsString()) {
        Log::Warning(Event::ParseStyle, "filter expression key must be a string");
        return empty;
    }

    Expression expression;
    expression.key = { value[1u].GetString(), value[1u].GetStringLength() };
    return expression;
}

template <class Expression>
Filter parseBinaryFilter(const JSValue& value) {
    Filter empty;

    if (value.Size() < 3) {
        Log::Warning(Event::ParseStyle, "filter expression must have 3 elements");
        return empty;
    }

    if (!value[1u].IsString()) {
        Log::Warning(Event::ParseStyle, "filter expression key must be a string");
        return empty;
    }

    Expression expression;
    expression.key = { value[1u].GetString(), value[1u].GetStringLength() };
    expression.value = parseValue(value[2u]);

    if (expression.key == "$type") {
        expression.value = parseFeatureType(expression.value);
    }

    return expression;
}

template <class Expression>
Filter parseSetFilter(const JSValue& value) {
    Filter empty;

    if (value.Size() < 2) {
        Log::Warning(Event::ParseStyle, "filter expression must at least 2 elements");
        return empty;
    }

    if (!value[1u].IsString()) {
        Log::Warning(Event::ParseStyle, "filter expression key must be a string");
        return empty;
    }

    Expression expression;
    expression.key = { value[1u].GetString(), value[1u].GetStringLength() };
    for (rapidjson::SizeType i = 2; i < value.Size(); ++i) {
        Value parsedValue = parseValue(value[i]);
        if (expression.key == "$type") {
            parsedValue = parseFeatureType(parsedValue);
        }
        expression.values.push_back(parsedValue);
    }
    return expression;
}

template <class Expression>
Filter parseCompoundFilter(const JSValue& value) {
    Expression expression;
    for (rapidjson::SizeType i = 1; i < value.Size(); ++i) {
        expression.filters.push_back(parseFilter(value[i]));
    }
    return expression;
}

Filter parseFilter(const JSValue& value) {
    Filter empty;

    if (!value.IsArray()) {
        Log::Warning(Event::ParseStyle, "filter expression must be an array");
        return empty;
    }

    if (value.Size() < 1) {
        Log::Warning(Event::ParseStyle, "filter expression must have at least 1 element");
        return empty;
    }

    if (!value[0u].IsString()) {
        Log::Warning(Event::ParseStyle, "filter operator must be a string");
        return empty;
    }

    std::string op = { value[0u].GetString(), value[0u].GetStringLength() };

    if (op == "==") {
        return parseBinaryFilter<EqualsFilter>(value);
    } else if (op == "!=") {
        return parseBinaryFilter<NotEqualsFilter>(value);
    } else if (op == ">") {
        return parseBinaryFilter<GreaterThanFilter>(value);
    } else if (op == ">=") {
        return parseBinaryFilter<GreaterThanEqualsFilter>(value);
    } else if (op == "<") {
        return parseBinaryFilter<LessThanFilter>(value);
    } else if (op == "<=") {
        return parseBinaryFilter<LessThanEqualsFilter>(value);
    } else if (op == "in") {
        return parseSetFilter<InFilter>(value);
    } else if (op == "!in") {
        return parseSetFilter<NotInFilter>(value);
    } else if (op == "all") {
        return parseCompoundFilter<AllFilter>(value);
    } else if (op == "any") {
        return parseCompoundFilter<AnyFilter>(value);
    } else if (op == "none") {
        return parseCompoundFilter<NoneFilter>(value);
    } else if (op == "has") {
        return parseUnaryFilter<HasFilter>(value);
    } else if (op == "!has") {
       return parseUnaryFilter<NotHasFilter>(value);
    } else {
        Log::Warning(Event::ParseStyle, "filter operator must be one of \"==\", \"!=\", \">\", \">=\", \"<\", \"<=\", \"in\", \"!in\", \"all\", \"any\", \"none\", \"has\", or \"!has\"");
        return empty;
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
