#include <mbgl/util/rapidjson.hpp>
#include <mbgl/util/string.hpp>

#include <algorithm>

namespace mbgl {

std::string formatJSONParseError(const std::string& json, const JSDocument& doc) {
    const size_t offset = std::min(doc.GetErrorOffset(), json.length());
    const size_t lastBreak = json.find_last_of('\n', offset);
    const size_t col = offset - (lastBreak == std::string::npos ? 0 : lastBreak);
    const size_t line = lastBreak == std::string::npos ? 1 : std::count(json.data(), json.data() + lastBreak, '\n') + 2;
    return "on line " + util::toString(line) + " col " + util::toString(col) + ": " +
           rapidjson::GetParseError_En(doc.GetParseError());
}

} // namespace mbgl


