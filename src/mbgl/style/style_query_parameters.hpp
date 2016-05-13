#pragma once

#include <mbgl/util/geo.hpp>

#include <vector>

namespace mbgl {

class TransformState;

class StyleQueryParameters {
public:
    const ScreenLineString& geometry;
    const TransformState& transformState;
    const optional<std::vector<std::string>>& layerIDs;
};

} // namespace mbgl
