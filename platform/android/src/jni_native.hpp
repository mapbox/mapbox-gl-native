#pragma once

#include <mbgl/util/util.hpp>

#include "jni.hpp"

namespace mbgl {
namespace android {

MBGL_PUBLIC void registerNatives(JavaVM* vm);

} // namespace android
} // namespace mbgl
