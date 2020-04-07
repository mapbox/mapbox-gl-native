#pragma once
#include <mbgl/math/wrap.hpp>
namespace mbgl {
namespace style {

// Could be made a template class if needed
// template <size_t Period>
// size_t period() const noexcept { return Period; }
class Rotation {
public:
    Rotation() = default;
    Rotation(double angle_) : angle(mbgl::util::wrap<double>(angle_, 0, period())) {}
    constexpr double period() const noexcept { return 360.0; }
    double getAngle() const noexcept { return angle; }

    friend bool operator==(const Rotation& lhs, const Rotation& rhs) { return lhs.angle == rhs.angle; }

    friend bool operator!=(const Rotation& lhs, const Rotation& rhs) { return !(lhs == rhs); }

private:
    double angle;
};

} // namespace style
} // namespace mbgl
