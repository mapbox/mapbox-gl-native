#pragma once

#include <mbgl/util/interpolate.hpp>
#include <mbgl/util/range.hpp>
#include <mbgl/util/unitbezier.hpp>

namespace mbgl {
namespace style {
namespace expression {

class ExponentialInterpolator {
public:
    ExponentialInterpolator(double base_) : base(base_) {}

    double base;
    
    double interpolationFactor(const Range<double>& inputLevels, const double input) const {
        return util::interpolationFactor(base,
                                         Range<float> {
                                            static_cast<float>(inputLevels.min),
                                            static_cast<float>(inputLevels.max)
                                         },
                                         input);
    }
    
    bool operator==(const ExponentialInterpolator& rhs) const {
        return base == rhs.base;
    }
};

class CubicBezierInterpolator {
public:
    CubicBezierInterpolator(double x1_, double y1_, double x2_, double y2_) : ub(x1_, y1_, x2_, y2_) {}
    
    double interpolationFactor(const Range<double>& inputLevels, const double input) const {
        return ub.solve(input / (inputLevels.max - inputLevels.min), 1e-6);
    }
    
    bool operator==(const CubicBezierInterpolator& rhs) const {
        return ub == rhs.ub;
    }
    
    util::UnitBezier ub;
};

using Interpolator = variant<ExponentialInterpolator, CubicBezierInterpolator>;

} // namespace expression
} // namespace style
} // namespace mbgl
