#include <mbgl/map/source_info.hpp>
#include <mbgl/util/mapbox.hpp>
#include <mbgl/util/string.hpp>
#include <mbgl/util/token.hpp>

namespace mbgl {

namespace {

void parse(const rapidjson::Value& value, std::vector<std::string>& target, const char* name) {
    if (!value.HasMember(name)) {
        return;
    }

    const rapidjson::Value& property = value[name];
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

void parse(const rapidjson::Value& value, std::string& target, const char* name) {
    if (!value.HasMember(name)) {
        return;
    }

    const rapidjson::Value& property = value[name];
    if (!property.IsString()) {
        return;
    }

    target = { property.GetString(), property.GetStringLength() };
}

void parse(const rapidjson::Value& value, uint16_t& target, const char* name) {
    if (!value.HasMember(name)) {
        return;
    }

    const rapidjson::Value& property = value[name];
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
void parse(const rapidjson::Value& value, std::array<float, N>& target, const char* name) {
    if (!value.HasMember(name)) {
        return;
    }

    const rapidjson::Value& property = value[name];
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

void SourceInfo::parseTileJSONProperties(const rapidjson::Value& value) {
    parse(value, tiles, "tiles");
    parse(value, min_zoom, "minzoom");
    parse(value, max_zoom, "maxzoom");
    parse(value, attribution, "attribution");
    parse(value, center, "center");
    parse(value, bounds, "bounds");
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
