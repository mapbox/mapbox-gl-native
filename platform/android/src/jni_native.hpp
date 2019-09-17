#pragma once

#include <mbgl/util/util.hpp>

#include "jni.hpp"

namespace mbgl {
namespace android {

MBGL_EXPORT void registerNatives(JavaVM* vm);

} // namespace android
} // namespace mbgl
