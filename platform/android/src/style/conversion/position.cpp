#include "position.hpp"

namespace mbgl {
namespace android {
namespace conversion {

Result<jni::Local<jni::Object<Position>>> Converter<jni::Local<jni::Object<Position>>, mbgl::style::Position>::operator()(jni::JNIEnv &env, const mbgl::style::Position &value) const {
    std::array<float, 3> cartPosition = value.getSpherical();
    return Position::fromPosition(env, cartPosition[0], cartPosition[1], cartPosition[2]);
}

Result<mbgl::style::Position> Converter<mbgl::style::Position, jni::Object<Position>>::operator()(jni::JNIEnv &env, const jni::Object<Position> &value) const {
    float radialCoordinate = Position::getRadialCoordinate(env, value);
    float azimuthalAngle = Position::getAzimuthalAngle(env, value);
    float polarAngle = Position::getPolarAngle(env, value);
    std::array<float, 3> cartPosition {{radialCoordinate, azimuthalAngle, polarAngle}};
    mbgl::style::Position position{};
    position.set(cartPosition);
    return position;
}

}
}
}
