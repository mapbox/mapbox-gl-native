#pragma once

#include <mbgl/util/chrono.hpp>
#include <mbgl/util/optional.hpp>

namespace mbgl {
namespace style {

class TransitionOptions {
public:
    optional<Duration> duration = {};
    optional<Duration> delay = {};

    TransitionOptions reverseMerge(const TransitionOptions& defaults) const {
        TransitionOptions options;

        options.duration = duration ? duration : defaults.duration;
        options.delay = delay ? delay : defaults.delay;

        return options;
    }

    explicit operator bool() const {
        return duration || delay;
    }
};

} // namespace style
} // namespace mbgl
