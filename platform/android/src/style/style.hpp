#pragma once

#include <mbgl/style/style.hpp>

#include <jni/jni.hpp>

namespace mbgl {
namespace android {

/**
 * Peer class for the Android Style holder.
 */
class Style {
public:

    // TODO move to style package
    static constexpr auto Name() { return "com/mapbox/mapboxsdk/maps/Style"; };

    Style(jni::JNIEnv &);

    ~Style();

    static jni::Class<Style> javaClass;

    // TODO add methods
    static void registerNative(jni::JNIEnv &);

private:

};

}
}