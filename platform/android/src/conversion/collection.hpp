#pragma once

#include "conversion.hpp"

#include <jni/jni.hpp>

#include <vector>

namespace mbgl {
namespace android {
namespace conversion {

std::vector<std::string> toVector(JNIEnv& env, jni::Array<jni::String> array);

}
}
}
