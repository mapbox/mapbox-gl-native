#include <mbgl/platform/log.hpp>
#include <mbgl/map/source_info.hpp>
#include <mbgl/util/mapbox.hpp>
#include <mbgl/util/string.hpp>
#include <mbgl/util/token.hpp>

#include <mapbox/geojsonvt.hpp>
#include <mapbox/geojsonvt/convert.hpp>

namespace mbgl {

namespace {

void parse(const JSValue& value, std::vector<std::string>& target, const char* name) {
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

void parse(const JSValue& value, std::string& target, const char* name) {
    if (!value.HasMember(name)) {
        return;
    }

    const JSValue& property = value[name];
    if (!property.IsString()) {
        return;
    }

    target = { property.GetString(), property.GetStringLength() };
}

void parse(const JSValue& value, uint16_t& target, const char* name) {
    if (!value.HasMember(name)) {
        return;
    }

    const JSValue& property = value[name];
    if (!property.IsUint()) {
        return;
    }

    unsigned int uint = property.GetUint();
    if (uint > std::numeric_limits<uint16_t>::max()) {
        return;
    }

    target = uint;
}

template <size_t N>
void parse(const JSValue& value, std::array<float, N>& target, const char* name) {
    if (!value.HasMember(name)) {
        return;
    }

    const JSValue& property = value[name];
    if (!property.IsArray() || property.Size() != N) {
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

// Destructor in implementation file because header only contains forward declarations.
SourceInfo::~SourceInfo() = default;

void SourceInfo::parseTileJSONProperties(const JSValue& value) {
    parse(value, tiles, "tiles");
    parse(value, min_zoom, "minzoom");
    parse(value, max_zoom, "maxzoom");
    parse(value, attribution, "attribution");
    parse(value, center, "center");
    parse(value, bounds, "bounds");
}

void SourceInfo::parseGeoJSON(const JSValue& value) {
    using namespace mapbox::geojsonvt;

    try {
        geojsonvt = std::make_unique<GeoJSONVT>(Convert::convert(value, 0));
    } catch (const std::exception& ex) {
        Log::Error(Event::ParseStyle, "Failed to parse GeoJSON data: %s", ex.what());
        // Create an empty GeoJSON VT object to make sure we're not infinitely waiting for
        // tiles to load.
        geojsonvt = std::make_unique<GeoJSONVT>(std::vector<ProjectedFeature>{});
    }
}

std::string SourceInfo::tileURL(const TileID& id, float pixelRatio) const {
    std::string result = tiles.at(0);
    result = util::mapbox::normalizeTileURL(result, url, type);
    result = util::replaceTokens(result, [&](const std::string& token) -> std::string {
        if (token == "z") {
            return util::toString(std::min(id.z, static_cast<int8_t>(max_zoom)));
        } else if (token == "x") {
            return util::toString(id.x);
        } else if (token == "y") {
            return util::toString(id.y);
        } else if (token == "prefix") {
            std::string prefix{ 2 };
            prefix[0] = "0123456789abcdef"[id.x % 16];
            prefix[1] = "0123456789abcdef"[id.y % 16];
            return prefix;
        } else if (token == "ratio") {
            return pixelRatio > 1.0 ? "@2x" : "";
        } else {
            return "";
        }
    });
    return result;
}

} // namespace mbgl
