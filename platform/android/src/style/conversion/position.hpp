#pragma once

#include "../../conversion/conversion.hpp"

#include <jni/jni.hpp>
#include <mbgl/style/position.hpp>
#include "../../jni/local_object.hpp"
#include "../position.hpp"

namespace mbgl {
namespace android {
namespace conversion {

template<>
struct Converter<jni::Object<Position>, mbgl::style::Position> {
    Result<jni::Object<Position>> operator()(jni::JNIEnv &env, const mbgl::style::Position &value) const {
        std::array<float, 3> cartPosition = value.getSpherical();
        return Position::fromPosition(env, cartPosition[0], cartPosition[1], cartPosition[2]);
    }
};

template<>
struct Converter<mbgl::style::Position, jni::Object<Position>> {
    Result<mbgl::style::Position> operator()(jni::JNIEnv &env, const jni::Object<Position> &value) const {
        float radialCoordinate = Position::getRadialCoordinate(env, value);
        float azimuthalAngle = Position::getAzimuthalAngle(env, value);
        float polarAngle = Position::getPolarAngle(env, value);
        std::array<float, 3> cartPosition {{radialCoordinate, azimuthalAngle, polarAngle}};
        mbgl::style::Position position{};
        position.set(cartPosition);
        return position;
    }
};

}
}
}