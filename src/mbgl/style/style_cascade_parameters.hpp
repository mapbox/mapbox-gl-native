#ifndef STYLE_CASCADE_PARAMETERS
#define STYLE_CASCADE_PARAMETERS

#include <mbgl/util/chrono.hpp>
#include <mbgl/style/types.hpp>

#include <vector>

namespace mbgl {

class PropertyTransition;

class StyleCascadeParameters {
public:
    StyleCascadeParameters(const std::vector<ClassID>& classes_,
                           const TimePoint& now_,
                           const PropertyTransition& defaultTransition_)
        : classes(classes_),
          now(now_),
          defaultTransition(defaultTransition_) {}

    std::vector<ClassID> classes;
    TimePoint now;
    PropertyTransition defaultTransition;
};

}

#endif
