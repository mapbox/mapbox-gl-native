#include <mbgl/renderer/group_by_layout.hpp>
#include <mbgl/style/conversion/stringify.hpp>
#include <mbgl/util/rapidjson.hpp>

#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

#include <unordered_map>

namespace mbgl {

std::string layoutKey(const style::Layer::Impl& impl) {
    using namespace style::conversion;

    rapidjson::StringBuffer s;
    rapidjson::Writer<rapidjson::StringBuffer> writer(s);

    writer.StartArray();
    writer.Uint64(reinterpret_cast<uint64_t>(impl.getTypeInfo()));
    writer.String(impl.source);
    writer.String(impl.sourceLayer);
    writer.Double(impl.minZoom);
    writer.Double(impl.maxZoom);
    writer.Uint(static_cast<uint32_t>(impl.visibility));
    stringify(writer, impl.filter);
    impl.stringifyLayout(writer);
    writer.EndArray();

    return s.GetString();
}

} // namespace mbgl
