#pragma once

#include <mbgl/util/optional.hpp>
#include <mbgl/util/size.hpp>
#include "quaternion.hpp"

namespace mbgl {

class LatLng;

namespace util {

class Camera {
public:
    Camera();

    vec3 getPosition() const;
    mat4 getCameraToWorld(double scale, bool flippedY) const;
    mat4 getWorldToCamera(double scale, bool flippedY) const;
    mat4 getCameraToClipPerspective(double fovy, double aspectRatio, double nearZ, double farZ) const;

    vec3 forward() const;
    vec3 right() const;
    vec3 up() const;

    const Quaternion& getOrientation() const { return orientation; }
    void getOrientation(double& pitch, double& bearing) const;
    void setOrientation(double pitch, double bearing);
    void setOrientation(const Quaternion& orientation_);
    void setPosition(const vec3& position);

    // Computes orientation using forward and up vectors of the provided coordinate frame.
    // Only bearing and pitch components will be used. Does not return a value if input is invalid
    static optional<Quaternion> orientationFromFrame(const vec3& forward, const vec3& up);

private:
    Quaternion orientation;
    mat4 transform; // Position (mercator) and orientation of the camera
};

} // namespace util
} // namespace mbgl