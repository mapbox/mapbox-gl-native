#pragma once

#include <mbgl/util/chrono.hpp>
#include <mbgl/util/optional.hpp>

namespace mbgl {
namespace style {

class TransitionOptions {
public:
    TransitionOptions(const optional<Duration>& duration_ = {}, const optional<Duration>& delay_ = {})
        : duration(duration_), delay(delay_) {}

    optional<Duration> duration;
    optional<Duration> delay;
};

} // namespace style
} // namespace mbgl
