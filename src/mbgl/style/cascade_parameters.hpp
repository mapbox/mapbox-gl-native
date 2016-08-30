#pragma once

#include <mbgl/util/chrono.hpp>
#include <mbgl/style/class_dictionary.hpp>
#include <mbgl/style/transition_options.hpp>

#include <vector>

namespace mbgl {
namespace style {

class CascadeParameters {
public:
    std::vector<ClassID> classes;
    TimePoint now;
    TransitionOptions transition;
};

} // namespace style
} // namespace mbgl
