#pragma once

#include <mbgl/util/chrono.hpp>
#include <mbgl/util/optional.hpp>

namespace mbgl {
namespace style {

class TransitionOptions {
public:
    TransitionOptions(optional<Duration> duration_ = {}, optional<Duration> delay_ = {})
        : duration(std::move(duration_)), delay(std::move(delay_)) {}

    optional<Duration> duration;
    optional<Duration> delay;
};

} // namespace style
} // namespace mbgl
