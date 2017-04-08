#pragma once

#include <mbgl/style/types.hpp>
#include <mbgl/util/optional.hpp>
#include <mbgl/util/constants.hpp>

#include <array>

#include <cassert>
#include <string>

namespace mbgl {
namespace style {
class Position {
public:
    Position() = default;
    Position(std::array<float, 3> position_)
        : radial(position_[0]), azimuthal(position_[1]), polar(position_[2]) {
        calculateCartesian();
    };

    friend bool operator==(const Position& lhs, const Position& rhs) {
        return lhs.radial == rhs.radial && lhs.azimuthal == rhs.azimuthal && lhs.polar == rhs.polar;
        // TODO should this address wrapping? or should somewhere else in Position address wrapping?
    }

    friend bool operator!=(const Position& lhs, const Position& rhs) {
        return !(lhs == rhs);
    }

    const std::array<float, 3> get() const {
        return { { x, y, z } };
    };

    const std::array<float, 3> getSpherical() const {
        return { { radial, azimuthal, polar } };
    };

    void set(std::array<float, 3> position_) {
        radial = position_[0];
        azimuthal = position_[1];
        polar = position_[2];
        calculateCartesian();
    };

private:
    float radial;
    float azimuthal;
    float polar;
    float x;
    float y;
    float z;

    void calculateCartesian() {
        auto _a = azimuthal;
        // We abstract "north"/"up" (compass-wise) to be 0° when really this is 90° (π/2): we
        // correct for that here
        _a += 90;
        _a *= util::DEG2RAD;
        auto _p = polar;
        _p *= util::DEG2RAD;

        x = radial * std::cos(_a) * std::sin(_p);
        y = radial * std::sin(_a) * std::sin(_p);
        z = radial * std::cos(_p);
    };
};
} // namespace style
} // namespace mbgl
