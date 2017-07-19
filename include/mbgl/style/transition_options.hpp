#pragma once

#include <mbgl/util/chrono.hpp>
#include <mbgl/util/optional.hpp>

namespace mbgl {
namespace style {

class TransitionOptions {
public:
    optional<Duration> duration;
    optional<Duration> delay;

    TransitionOptions(optional<Duration> duration_ = {},
                      optional<Duration> delay_ = {})
        : duration(std::move(duration_)),
          delay(std::move(delay_)) {}

    TransitionOptions reverseMerge(const TransitionOptions& defaults) const {
        return {
            duration ? duration : defaults.duration,
            delay ? delay : defaults.delay
        };
    }

    bool isDefined() const {
        return duration || delay;
    }
};

} // namespace style
} // namespace mbgl
