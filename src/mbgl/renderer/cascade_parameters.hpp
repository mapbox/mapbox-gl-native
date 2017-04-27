#pragma once

#include <mbgl/util/chrono.hpp>
#include <mbgl/style/class_dictionary.hpp>
#include <mbgl/style/transition_options.hpp>

#include <vector>

namespace mbgl {

class CascadeParameters {
public:
    std::vector<style::ClassID> classes;
    TimePoint now;
    style::TransitionOptions transition;
};

} // namespace mbgl
