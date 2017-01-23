#include <mbgl/style/group_by_layout.hpp>
#include <mbgl/style/layer.hpp>
#include <mbgl/style/layer_impl.hpp>
#include <mbgl/style/conversion/stringify.hpp>
#include <mbgl/util/rapidjson.hpp>

#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

#include <unordered_map>

namespace mbgl {
namespace style {

std::string layoutKey(const Layer& layer) {
    using namespace conversion;

    rapidjson::StringBuffer s;
    rapidjson::Writer<rapidjson::StringBuffer> writer(s);

    writer.StartArray();
    writer.Uint(static_cast<uint32_t>(layer.type));
    writer.String(layer.baseImpl->source);
    writer.String(layer.baseImpl->sourceLayer);
    writer.Double(layer.baseImpl->minZoom);
    writer.Double(layer.baseImpl->maxZoom);
    writer.Uint(static_cast<uint32_t>(layer.baseImpl->visibility));
    stringify(writer, layer.baseImpl->filter);
    layer.baseImpl->stringifyLayout(writer);
    writer.EndArray();

    return s.GetString();
}

std::vector<std::vector<const Layer*>> groupByLayout(const std::vector<std::unique_ptr<Layer>>& layers) {
    std::unordered_map<std::string, std::vector<const Layer*>> map;
    for (auto& layer : layers) {
        map[layoutKey(*layer)].push_back(layer.get());
    }

    std::vector<std::vector<const Layer*>> result;
    for (auto& pair : map) {
        result.push_back(pair.second);
    }

    return result;
}

} // namespace style
} // namespace mbgl
