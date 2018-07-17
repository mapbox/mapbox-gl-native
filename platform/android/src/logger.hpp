#pragma once

#include <mbgl/util/event.hpp>
#include <jni/jni.hpp>
#include "bitmap.hpp"

namespace mbgl {
namespace android {

class Logger {
public:
   static constexpr auto Name() {
       return "com/mapbox/mapboxsdk/log/Logger";
   };
   static void registerNative(jni::JNIEnv&);

   static void log(jni::JNIEnv&, EventSeverity severity, const std::string &msg);

private:
   static jni::Class<Logger> _class;
};

} // namespace android
} // namespace mbgl