#include <mbgl/style/conversion/geojson.hpp>

#include <string>
namespace mbgl {
namespace style {
namespace conversion {

template <>
optional<GeoJSON> Converter<GeoJSON>::operator()(const v8::Local<v8::Value>& value, Error& error) const {
    Nan::JSON JSON;
    std::string string = *Nan::Utf8String(JSON.Stringify(value->ToObject()).ToLocalChecked());
    return convert<GeoJSON>(string, error);
}

} // namespace conversion
} // namespace style
} // namespace mbgl
