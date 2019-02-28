#pragma once

#include <mbgl/gfx/types.hpp>
#include <mbgl/util/range.hpp>

namespace mbgl {
namespace gfx {

class DepthMode {
public:
    DepthFunctionType func;
    DepthMaskType mask;
    Range<float> range;

    static DepthMode disabled() {
       return DepthMode { DepthFunctionType::Always, DepthMaskType::ReadOnly, { 0.0, 1.0 } };
    }
};

} // namespace gfx
} // namespace mbgl
