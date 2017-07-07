#pragma once

#include <mbgl/util/image.hpp>

namespace mbgl {
namespace util {

/*
    C++ port of https://github.com/mapbox/tiny-sdf, which is in turn based on the
    Felzenszwalb/Huttenlocher distance transform paper (https://cs.brown.edu/~pff/papers/dt-final.pdf).
    Note there exists an alternative C++ implementation from the paper’s authors at
    https://cs.brown.edu/~pff/dt/, which this implementation is not based on.
 
    Takes an alpha channel raster input and transforms it into an alpha channel
    Signed Distance Field (SDF) output of the same dimensions.
*/
AlphaImage transformRasterToSDF(const AlphaImage& rasterInput, double radius, double cutoff);

} // namespace util
} // namespace mbgl
