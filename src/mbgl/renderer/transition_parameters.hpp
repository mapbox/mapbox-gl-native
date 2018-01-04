#pragma once

#include <mbgl/util/chrono.hpp>
#include <mbgl/style/transition_options.hpp>

#include <vector>

namespace mbgl {

class TransitionParameters {
public:
    TimePoint now;
    style::TransitionOptions transition;
};

} // namespace mbgl
