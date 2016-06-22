#include <mbgl/style/conversion.hpp>

namespace mbgl {
namespace style {
namespace conversion {

Result<Value> normalizeFilterValue(const std::string& key, const optional<Value>& value) {
    if (!value) {
        return Error { "filter expression value must be a boolean, number, or string" };
    } else if (key != "$type") {
        return *value;
    } else if (*value == std::string("Point")) {
        return Value(uint64_t(FeatureType::Point));
    } else if (*value == std::string("LineString")) {
        return Value(uint64_t(FeatureType::LineString));
    } else if (*value == std::string("Polygon")) {
        return Value(uint64_t(FeatureType::Polygon));
    } else {
        return Error { "value for $type filter must be Point, LineString, or Polygon" };
    }
}

} // namespace conversion
} // namespace style
} // namespace mbgl
