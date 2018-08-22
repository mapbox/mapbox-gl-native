#pragma once

#include "layer.hpp"
#include <mbgl/style/layer.hpp>
#include <jni/jni.hpp>

namespace mbgl {
namespace android {

class UnknownLayer : public Layer {
public:
    using SuperTag = Layer;
    static constexpr auto Name() { return "com/mapbox/mapboxsdk/style/layers/UnknownLayer"; };

    static void registerNative(jni::JNIEnv&);

    UnknownLayer(mbgl::Map&, mbgl::style::Layer&);

    UnknownLayer(mbgl::Map&, std::unique_ptr<mbgl::style::Layer>);

    ~UnknownLayer() = default;

    jni::Local<jni::Object<Layer>> createJavaPeer(jni::JNIEnv&);

}; // class UnknownLayer

} // namespace android
} // namespace mbgl
