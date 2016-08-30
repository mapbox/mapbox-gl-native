#pragma once

namespace mbgl {

class PlacementConfig {
public:
    PlacementConfig(float angle_ = 0, float pitch_ = 0, bool debug_ = false)
        : angle(angle_), pitch(pitch_), debug(debug_) {
    }

    bool operator==(const PlacementConfig& rhs) const {
        return angle == rhs.angle && pitch == rhs.pitch && debug == rhs.debug;
    }

    bool operator!=(const PlacementConfig& rhs) const {
        return !operator==(rhs);
    }

public:
    float angle;
    float pitch;
    bool debug;
};

} // namespace mbgl
