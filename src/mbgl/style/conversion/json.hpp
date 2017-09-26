#include <mbgl/style/conversion.hpp>
#include <mbgl/style/rapidjson_conversion.hpp>

#include <string>
#include <sstream>

namespace mbgl {
namespace style {
namespace conversion {

template <class T, class...Args>
optional<T> convertJSON(const std::string& json, Error& error, Args&&...args) {
    JSDocument document;
    document.Parse<0>(json.c_str());

    if (document.HasParseError()) {
        std::stringstream message;
        message << document.GetErrorOffset() << " - " << rapidjson::GetParseError_En(document.GetParseError());
        error = { message.str() };
        return {};
    }

    return convert<T>(document, error, std::forward<Args>(args)...);
}

} // namespace conversion
} // namespace style
} // namespace mbgl
