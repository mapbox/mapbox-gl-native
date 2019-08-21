#pragma once

#include <mbgl/style/conversion.hpp>
#include <mbgl/util/feature.hpp>

namespace mbgl {

class RenderTile;
using namespace style::conversion;

class SourceFeatureState {
public:
    SourceFeatureState() = default;
    ~SourceFeatureState() = default;

    void updateState(const optional<std::string>& sourceLayerID, const std::string& featureID, const FeatureState& newState);
    void getState(FeatureState& result, const optional<std::string>& sourceLayerID, const std::string& featureID) const;

    void coalesceChanges(std::vector<RenderTile>& tiles);

private:
    LayerFeatureStates currentStates;
    LayerFeatureStates stateChanges;
};

} // namespace mbgl
