#include <mbgl/style/parser.hpp>
#include <mbgl/style/sources/raster_source.hpp>
#include <mbgl/style/sources/vector_source.hpp>
#include <mbgl/style/sources/geojson_source.hpp>
#include <mbgl/style/layer_impl.hpp>
#include <mbgl/style/layers/fill_layer.hpp>
#include <mbgl/style/layers/line_layer.hpp>
#include <mbgl/style/layers/circle_layer.hpp>
#include <mbgl/style/layers/symbol_layer.hpp>
#include <mbgl/style/layers/raster_layer.hpp>
#include <mbgl/style/layers/background_layer.hpp>

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
            source = RasterSource::parse(id, sourceVal);
            break;
        }

        case SourceType::Vector:
            source = VectorSource::parse(id, sourceVal);
            break;

        case SourceType::GeoJSON:
            source = GeoJSONSource::parse(id, sourceVal);
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

void Parser::parseVisibility(Layer& layer, const JSValue& value) {
    Layer::Impl& impl = *layer.baseImpl;

    if (!value.HasMember("visibility")) {
        return;
    }

    if (!value["visibility"].IsString()) {
        Log::Warning(Event::ParseStyle, "value of 'visibility' must be a string");
        return;
    }

    const auto enumValue = Enum<VisibilityType>::toEnum({ value["visibility"].GetString(), value["visibility"].GetStringLength() });
    if (!enumValue) {
        Log::Warning(Event::ParseStyle, "value of 'visibility' must be a valid enumeration value");
        return;
    }

    impl.visibility = *enumValue;
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
