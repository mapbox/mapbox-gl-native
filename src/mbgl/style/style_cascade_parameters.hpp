#ifndef STYLE_CASCADE_PARAMETERS
#define STYLE_CASCADE_PARAMETERS

#include <mbgl/util/chrono.hpp>
#include <mbgl/style/types.hpp>

#include <vector>

namespace mbgl {

class PropertyTransition;

class StyleCascadeParameters {
public:
    std::vector<ClassID> classes;
    TimePoint now;
    PropertyTransition userDefinedTransition;
};

} // namespace mbgl

#endif
