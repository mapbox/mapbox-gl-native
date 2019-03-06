#pragma once

#include <rapidjson/document.h>
#include <rapidjson/error/en.h>

#include <string>

namespace mbgl {

using JSDocument = rapidjson::GenericDocument<rapidjson::UTF8<>, rapidjson::CrtAllocator>;
using JSValue = rapidjson::GenericValue<rapidjson::UTF8<>, rapidjson::CrtAllocator>;

std::string formatJSONParseError(const std::string&, const JSDocument&);

} // namespace mbgl
