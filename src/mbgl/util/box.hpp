#ifndef MBGL_UTIL_BOX
#define MBGL_UTIL_BOX

#include <mbgl/util/vec.hpp>

namespace mbgl {

struct box {
    vec2<double> tl, tr, bl, br;
    vec2<double> center;
};

}

#endif
