#include <mbgl/renderer/source_state.hpp>
#include <mbgl/style/conversion_impl.hpp>
#include <mbgl/renderer/render_tile.hpp>
#include <mbgl/util/logging.hpp>

namespace mbgl {

void SourceFeatureState::updateState(const optional<std::string>& sourceLayerID, const std::string& featureID, const FeatureState& newState) {
    std::string sourceLayer = sourceLayerID.value_or(std::string());
    for (const auto& state : newState) {
        auto& layerStates = stateChanges[sourceLayer];
        auto& featureStates = layerStates[featureID];
        featureStates[state.first] = state.second;
    }
}

void SourceFeatureState::getState(FeatureState& result, const optional<std::string>& sourceLayerID, const std::string& featureID) const {
    std::string sourceLayer = sourceLayerID.value_or(std::string());
    FeatureState current, changes;
    auto layerStates = currentStates.find(sourceLayer);
    if (layerStates != currentStates.end()) {
        const auto currentStateEntry = layerStates->second.find(featureID);
        if (currentStateEntry != layerStates->second.end()) {
            current = currentStateEntry->second;
        }
    }

    layerStates = stateChanges.find(sourceLayer);
    if (layerStates != stateChanges.end()) {
        const auto stateChangesEntry = layerStates->second.find(featureID);
        if (stateChangesEntry != layerStates->second.end()) {
            changes = stateChangesEntry->second;
        }
    }
    result = std::move(changes);
    result.insert(current.begin(), current.end());
}

void SourceFeatureState::coalesceChanges(std::vector<RenderTile>& tiles) {
    LayerFeatureStates changes;
    for (const auto& layerStatesEntry : stateChanges) {
        const auto& sourceLayer = layerStatesEntry.first;
        FeatureStates layerStates;
        for (const auto& featureStatesEntry : stateChanges[sourceLayer]) {
            const auto& featureID = featureStatesEntry.first;
            for (const auto& stateEntry : stateChanges[sourceLayer][featureID]) {
                const auto& stateKey = stateEntry.first;
                const auto& stateVal = stateEntry.second;

                auto currentState = currentStates[sourceLayer][featureID].find(stateKey);
                if (currentState != currentStates[sourceLayer][featureID].end()) {
                    currentState->second = stateVal;
                } else {
                    currentStates[sourceLayer][featureID].insert(std::make_pair(stateKey, stateVal));
                }
            }
            layerStates[featureID] = currentStates[sourceLayer][featureID];
        }
        changes[sourceLayer] = std::move(layerStates);
    }
    stateChanges.clear();
    if (changes.empty()) return;

    for (auto& tile : tiles) {
        tile.setFeatureState(changes);
    }
}

} // namespace mbgl
