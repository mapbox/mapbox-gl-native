#include <mbgl/style/style_source.hpp>
#include <mbgl/platform/platform.hpp>
#include <mbgl/platform/log.hpp>

#include <limits>

namespace mbgl {

void parse(const rapidjson::Value& value, std::vector<std::string>& target, const char *name) {
    if (!value.HasMember(name))
        return;

    const rapidjson::Value& property = value[name];
    if (!property.IsArray())
        return;

    for (rapidjson::SizeType i = 0; i < property.Size(); i++)
        if (!property[i].IsString())
            return;

    for (rapidjson::SizeType i = 0; i < property.Size(); i++)
        target.emplace_back(std::string(property[i].GetString(), property[i].GetStringLength()));
}

void parse(const rapidjson::Value& value, std::string& target, const char* name) {
    if (!value.HasMember(name))
        return;

    const rapidjson::Value& property = value[name];
    if (!property.IsString())
        return;

    target = { property.GetString(), property.GetStringLength() };
}

void parse(const rapidjson::Value& value, uint16_t& target, const char* name) {
    if (!value.HasMember(name))
        return;

    const rapidjson::Value& property = value[name];
    if (!property.IsUint())
        return;

    unsigned int uint = property.GetUint();
    if (uint > std::numeric_limits<uint16_t>::max())
        return;

    target = uint;
}

template <size_t N>
void parse(const rapidjson::Value& value, std::array<float, N>& target, const char* name) {
    if (!value.HasMember(name))
        return;

    const rapidjson::Value& property = value[name];
    if (!property.IsArray() || property.Size() != N)
        return;

    for (rapidjson::SizeType i = 0; i < property.Size(); i++)
        if (!property[i].IsNumber())
            return;

    for (rapidjson::SizeType i = 0; i < property.Size(); i++)
        target[i] = property[i].GetDouble();
}

void SourceInfo::parseTileJSONProperties(const rapidjson::Value& value) {
    parse(value, tiles, "tiles");
    parse(value, min_zoom, "minzoom");
    parse(value, max_zoom, "maxzoom");
    parse(value, attribution, "attribution");
    parse(value, center, "center");
    parse(value, bounds, "bounds");
}

}
