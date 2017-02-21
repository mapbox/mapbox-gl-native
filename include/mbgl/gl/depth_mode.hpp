#pragma once

#include <mbgl/util/range.hpp>

namespace mbgl {
namespace gl {

class DepthMode {
public:
    enum Function {
        Never        = 0x0200,
        Less         = 0x0201,
        Equal        = 0x0202,
        LessEqual    = 0x0203,
        Greater      = 0x0204,
        NotEqual     = 0x0205,
        GreaterEqual = 0x0206,
        Always       = 0x0207
    };

    enum Mask : bool {
        ReadOnly = false,
        ReadWrite = true
    };

    Function func;
    Mask mask;
    Range<float> range;

    static DepthMode disabled() {
       return DepthMode { Always, ReadOnly, { 0.0, 1.0 } };
    }
};

} // namespace gl
} // namespace mbgl
