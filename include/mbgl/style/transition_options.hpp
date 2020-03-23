#pragma once

#include <mbgl/util/chrono.hpp>
#include <mbgl/util/optional.hpp>

#include <mapbox/compatibility/value.hpp>

namespace mbgl {
namespace style {

class TransitionOptions {
public:
    optional<Duration> duration;
    optional<Duration> delay;
    bool enablePlacementTransitions;

    TransitionOptions(optional<Duration> duration_ = {},
                      optional<Duration> delay_ = {},
                      bool enablePlacementTransitions_ = true)
        : duration(std::move(duration_)),
          delay(std::move(delay_)),
          enablePlacementTransitions(enablePlacementTransitions_) {}

    TransitionOptions reverseMerge(const TransitionOptions& defaults) const {
        return {
            duration ? duration : defaults.duration,
            delay ? delay : defaults.delay,
            enablePlacementTransitions
        };
    }

    bool isDefined() const {
        return duration || delay;
    }

    mapbox::base::Value serialize() const {
        mapbox::base::ValueObject result;
        if (duration) {
            result.emplace("duration", std::chrono::duration_cast<std::chrono::milliseconds>(*duration).count());
        }
        if (delay) {
            result.emplace("delay", std::chrono::duration_cast<std::chrono::milliseconds>(*delay).count());
        }
        return result;
    }
};

} // namespace style
} // namespace mbgl
