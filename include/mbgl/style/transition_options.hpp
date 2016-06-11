#pragma once

#include <mbgl/util/chrono.hpp>
#include <mbgl/util/optional.hpp>

namespace mbgl {
namespace style {

struct TransitionOptions {
    optional<Duration> duration = {};
    optional<Duration> delay = {};
};

} // namespace style
} // namespace mbgl
