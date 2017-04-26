#pragma once

//#include <mbgl/util/noncopyable.hpp>
//#include <mbgl/util/optional.hpp>
#include <mbgl/style/types.hpp>
#include <mbgl/style/property_value.hpp>
#include <mbgl/util/constants.hpp>
#include <mbgl/util/mat3.hpp>

#include <memory>
//#include <string>

namespace mbgl {
namespace style {

class Position {
public:
    Position() = default;
    Position(float radial_, float azimuthal_, float polar_) : radial(radial_), azimuthal(azimuthal_), polar(polar_) { calculateCartesian(); }
    Position(std::array<float, 3> position_) : radial(position_[0]), azimuthal(position_[1]), polar(position_[2]) { calculateCartesian(); }

    const std::array<float, 3> get() const {
        return {{ x, y, z }};
    }
    void set(std::array<float, 3> position_) {
        radial = position_[0];
        azimuthal = position_[1];
        polar = position_[2];
        calculateCartesian();
    }

private:
    float radial;
    float azimuthal;
    float polar;
    float x;
    float y;
    float z;

    void calculateCartesian() {
        auto _a = azimuthal;
        // We abstract "north"/"up" (compass-wise) to be 0° when really this is 90° (π/2): we correct for that here
        _a += 90;
        _a *= util::DEG2RAD;
        auto _p = polar;
        _p *= util::DEG2RAD;

        x = radial * std::cos(_a) * std::sin(_p);
        y = radial * std::sin(_a) * std::sin(_p);
        z = radial * std::cos(_p);
    };

};

    // TODO transitions!!!!!!!!!

/**
 * The runtime representation of a [light object](https://www.mapbox.com/mapbox-gl-js/style-spec/#light) from the Mapbox Style
 * Specification.
 */
class Light {
public:
    Light() = default;
    Light(LightAnchorType anchor_, std::array<float, 3> position_, Color color_, float intensity_)
    : anchor(std::move(anchor_)), position(std::move(position_)), color(std::move(color_)), intensity(intensity_) {
    }

    static LightAnchorType getDefaultAnchor();
    LightAnchorType getAnchor() const {
        return anchor;
    }
    void setAnchor(LightAnchorType anchor_) {
        anchor = anchor_;
    }

    static std::array<float, 3> getDefaultPosition();
    std::array<float, 3> getPosition() const {
        return position.get();
    }
    void setPosition(std::array<float, 3> position_) {
        position.set(position_);
    }

    static std::array<float, 3> getDefaultColor();
    std::array<float, 3> getColor() const {
        return {{ color.r, color.g, color.b }};     // TODO confirm that this is premultiplied
    }
    void setColor(Color color_) {
        color = color_;
    }

    static float getDefaultIntensity();
    float getIntensity() const {
        return intensity;
    }
    void setIntensity(float intensity_) {
        intensity = intensity_;
    }

private:
    LightAnchorType anchor{ LightAnchorType::Viewport };
    Position position{ 1.15, 210, 30 };
    Color color{ Color::white() };
    float intensity{ 0.5 };
};

} // namespace style
} // namespace mbgl
