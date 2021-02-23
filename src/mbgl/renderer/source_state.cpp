#include <mbgl/renderer/render_tile.hpp>
#include <mbgl/renderer/source_state.hpp>
#include <mbgl/style/conversion_impl.hpp>
#include <mbgl/util/logging.hpp>

namespace mbgl {

void SourceFeatureState::updateState(const optional<std::string>& sourceLayerID, const std::string& featureID,
                                     const FeatureState& newState) {
    std::string sourceLayer = sourceLayerID.value_or(std::string());
    for (const auto& state : newState) {
        auto& layerStates = stateChanges[sourceLayer];
        auto& featureStates = layerStates[featureID];
        featureStates[state.first] = state.second;
    }
}

void SourceFeatureState::getState(FeatureState& result, const optional<std::string>& sourceLayerID,
                                  const std::string& featureID) const {
    std::string sourceLayer = sourceLayerID.value_or(std::string());
    FeatureState current;
    FeatureState changes;
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

    for (const auto& layerStatesEntry : deletedStates) {
        const auto& sourceLayer = layerStatesEntry.first;
        FeatureStates layerStates = {{}, {}};

        if (deletedStates[sourceLayer].empty()) {
            for (const auto& featureStatesEntry : currentStates[sourceLayer]) {
                const auto& featureID = featureStatesEntry.first;
                layerStates[featureID] = {};
                currentStates[sourceLayer][featureID] = {};
            }
        } else {
            for (const auto& feature : deletedStates[sourceLayer]) {
                const auto& featureID = feature.first;
                bool deleteWholeFeatureState = deletedStates[sourceLayer][featureID].empty();
                if (deleteWholeFeatureState) {
                    currentStates[sourceLayer][featureID] = {};
                } else {
                    for (const auto& stateEntry : deletedStates[sourceLayer][featureID]) {
                        currentStates[sourceLayer][featureID].erase(stateEntry.first);
                    }
                }
                layerStates[featureID] = currentStates[sourceLayer][featureID];
            }
        }
        changes[sourceLayer] = std::move(layerStates);
    }

    stateChanges.clear();
    deletedStates.clear();

    if (changes.empty()) {
        return;
    }

    for (auto& tile : tiles) {
        tile.setFeatureState(changes);
    }
}

void SourceFeatureState::removeState(const optional<std::string>& sourceLayerID, const optional<std::string>& featureID,
                                     const optional<std::string>& stateKey) {
    std::string sourceLayer = sourceLayerID.value_or(std::string());

    bool sourceLayerDeleted = (deletedStates.count(sourceLayer) > 0) && deletedStates[sourceLayer].empty();
    if (sourceLayerDeleted) {
        return;
    }

    if (stateKey && featureID) {
        if ((deletedStates.count(sourceLayer) == 0) && (deletedStates[sourceLayer].count(*featureID)) == 0) {
            deletedStates[sourceLayer][*featureID][*stateKey] = {};
        }
    } else if (featureID) {
        bool updateInQueue =
            (stateChanges.count(sourceLayer) != 0U) && (stateChanges[sourceLayer].count(*featureID) != 0U);
        if (updateInQueue) {
            for (const auto& changeEntry : stateChanges[sourceLayer][*featureID]) {
                deletedStates[sourceLayer][*featureID][changeEntry.first] = {};
            }
        } else {
            deletedStates[sourceLayer][*featureID] = {};
        }
    } else {
        deletedStates[sourceLayer] = {};
    }
}

} // namespace mbgl
