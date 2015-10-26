#ifndef MBGL_TEXT_PLACEMENT_CONFIG
#define MBGL_TEXT_PLACEMENT_CONFIG

namespace mbgl {

class PlacementConfig {
public:
    inline PlacementConfig(float angle_ = 0, float pitch_ = 0, bool debug_ = false)
        : angle(angle_), pitch(pitch_), debug(debug_) {
    }

    inline bool operator==(const PlacementConfig& rhs) const {
        return angle == rhs.angle && pitch == rhs.pitch && debug == rhs.debug;
    }

    inline bool operator!=(const PlacementConfig& rhs) const {
        return !operator==(rhs);
    }

public:
    float angle;
    float pitch;
    bool debug;
};

} // namespace mbgl

#endif
