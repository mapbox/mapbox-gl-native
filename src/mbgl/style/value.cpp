#include <mbgl/style/value.hpp>
#include <mbgl/util/string.hpp>

std::string mbgl::toString(const mbgl::Value& value) {
    if (value.is<std::string>()) return value.get<std::string>();
    else if (value.is<bool>()) return value.get<bool>() ? "true" : "false";
    else if (value.is<int64_t>()) return util::toString(value.get<int64_t>());
    else if (value.is<uint64_t>()) return util::toString(value.get<uint64_t>());
    else if (value.is<double>()) return util::toString(value.get<double>());
    else return "null";
}

mbgl::Value mbgl::parseValue(const rapidjson::Value& value) {
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
