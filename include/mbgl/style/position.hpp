#pragma once

#include <mbgl/util/constants.hpp>

#include <array>

namespace mbgl {
namespace style {
class Position {
public:
    Position() = default;
    Position(std::array<float, 3>& position_)
        : radial(position_[0]), azimuthal(position_[1]), polar(position_[2]) {
        calculateCartesian();
    };

    friend bool operator==(const Position& lhs, const Position& rhs) {
        return lhs.radial == rhs.radial && lhs.azimuthal == rhs.azimuthal && lhs.polar == rhs.polar;
        // TODO this doesn't address wrapping, which would be better addressed by comparing cartesian coordinates but being calculated floats are ont to be trusted.
    }

    friend bool operator!=(const Position& lhs, const Position& rhs) {
        return !(lhs == rhs);
    }

    const std::array<float, 3> getCartesian() const {
        return { { x, y, z } };
    };

    const std::array<float, 3> getSpherical() const {
        return { { radial, azimuthal, polar } };
    };

    void set(std::array<float, 3>& position_) {
        radial = position_[0];
        azimuthal = position_[1];
        polar = position_[2];
        calculateCartesian();
    };

    // Utility function to be used only during interpolation; this leaves spherical coordinates undefined.
    void setCartesian(std::array<float, 3>& position_) {
        x = position_[0];
        y = position_[1];
        z = position_[2];
    }

private:
    float radial;
    float azimuthal;
    float polar;
    float x;
    float y;
    float z;

    void calculateCartesian() {
        // We abstract "north"/"up" (compass-wise) to be 0° when really this is 90° (π/2): we
        // correct for that here
        const float _a = (azimuthal + 90) * util::DEG2RAD;
        const float _p = polar * util::DEG2RAD;

        x = radial * std::cos(_a) * std::sin(_p);
        y = radial * std::sin(_a) * std::sin(_p);
        z = radial * std::cos(_p);
    };
};
} // namespace style
} // namespace mbgl
