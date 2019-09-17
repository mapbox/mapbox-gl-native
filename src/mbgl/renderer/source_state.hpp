#pragma once

#include <mbgl/style/conversion.hpp>
#include <mbgl/util/feature.hpp>

namespace mbgl {

class RenderTile;

class SourceFeatureState {
public:
    SourceFeatureState() = default;
    ~SourceFeatureState() = default;

    void updateState(const optional<std::string>& sourceLayerID, const std::string& featureID,
                     const FeatureState& newState);
    void getState(FeatureState& result, const optional<std::string>& sourceLayerID, const std::string& featureID) const;
    void removeState(const optional<std::string>& sourceLayerID, const optional<std::string>& featureID,
                     const optional<std::string>& stateKey);

    void coalesceChanges(std::vector<RenderTile>& tiles);

private:
    LayerFeatureStates currentStates;
    LayerFeatureStates stateChanges;
    LayerFeatureStates deletedStates;
};

} // namespace mbgl
