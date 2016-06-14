#pragma once

#include <mbgl/util/geo.hpp>

#include <vector>

namespace mbgl {

class TransformState;

namespace style {

class QueryParameters {
public:
    const ScreenLineString& geometry;
    const TransformState& transformState;
    const optional<std::vector<std::string>>& layerIDs;
};

} // namespace style
} // namespace mbgl
