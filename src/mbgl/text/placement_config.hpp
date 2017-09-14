#pragma once

#include <mbgl/util/constants.hpp>

namespace mbgl {

class PlacementConfig {
public:
    PlacementConfig(float angle_ = 0, float pitch_ = 0, float cameraToCenterDistance_ = 0, float cameraToTileDistance_ = 0, bool debug_ = false)
        : angle(angle_), pitch(pitch_), cameraToCenterDistance(cameraToCenterDistance_), cameraToTileDistance(cameraToTileDistance_), debug(debug_) {
    }

    bool operator==(const PlacementConfig& rhs) const {
        return angle == rhs.angle &&
            pitch == rhs.pitch &&
            debug == rhs.debug &&
            ((pitch * util::RAD2DEG < 25) ||
             (cameraToCenterDistance == rhs.cameraToCenterDistance && cameraToTileDistance == rhs.cameraToTileDistance));
    }

    bool operator!=(const PlacementConfig& rhs) const {
        return !operator==(rhs);
    }

public:
    float angle;
    float pitch;
    float cameraToCenterDistance;
    float cameraToTileDistance;
    bool debug;
};

} // namespace mbgl
