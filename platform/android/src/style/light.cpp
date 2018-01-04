// This file is generated. Edit android/platform/scripts/generate-style-code.js, then run `make android-style-code`.

#include <mbgl/map/map.hpp>
#include "light.hpp"
#include "conversion/transition_options.hpp"
#include "conversion/position.hpp"

namespace mbgl {
namespace android {

Light::Light(mbgl::Map& coreMap, mbgl::style::Light& coreLight)
    : light(coreLight) , map(&coreMap) {
}

static Light* initializeLightPeer(mbgl::Map& map, mbgl::style::Light& coreLight) {
    return new Light(map, coreLight);
}

jni::jobject* Light::createJavaLightPeer(jni::JNIEnv& env, Map& map, mbgl::style::Light& coreLight) {
    std::unique_ptr<Light> peerLight = std::unique_ptr<Light>(initializeLightPeer(map, coreLight));
    jni::jobject* result = peerLight->createJavaPeer(env);
    peerLight.release();
    return result;
}

jni::Class<Light> Light::javaClass;

jni::jobject* Light::createJavaPeer(jni::JNIEnv& env) {
    static auto constructor = Light::javaClass.template GetConstructor<jni::jlong>(env);
    return Light::javaClass.New(env, constructor, reinterpret_cast<jni::jlong>(this));
}

void Light::setAnchor(jni::JNIEnv& env, jni::String property) {
    std::string anchorStr = jni::Make<std::string>(env, property);
    if (anchorStr.compare("map") == 0) {
        light.setAnchor(LightAnchorType::Map);
    } else if (anchorStr.compare("viewport") == 0) {
        light.setAnchor(LightAnchorType::Viewport);
    }
}

jni::String Light::getAnchor(jni::JNIEnv& env) {
    auto anchorType = light.getAnchor();
    if (anchorType == LightAnchorType::Map) {
        return jni::Make<jni::String>(env, "map");
    } else {
        return jni::Make<jni::String>(env, "viewport");
    }
}

void Light::setPosition(jni::JNIEnv& env, jni::Object<Position> jposition) {
    using namespace mbgl::android::conversion;
    auto position = *convert<mbgl::style::Position>(env, jposition);
    light.setPosition(position);
}

jni::Object<Position> Light::getPosition(jni::JNIEnv& env) {
    using namespace mbgl::android::conversion;
    mbgl::style::Position position = light.getPosition().asConstant();
    return *convert<jni::Object<Position>>(env, position);
}

jni::Object<TransitionOptions> Light::getPositionTransition(jni::JNIEnv& env) {
   using namespace mbgl::android::conversion;
   mbgl::style::TransitionOptions options = light.getPositionTransition();
   return *convert<jni::Object<TransitionOptions>>(env, options);
}

void Light::setPositionTransition(jni::JNIEnv&, jlong duration, jlong delay) {
   mbgl::style::TransitionOptions options;
   options.duration.emplace(mbgl::Milliseconds(duration));
   options.delay.emplace(mbgl::Milliseconds(delay));
   light.setPositionTransition(options);
}

void Light::setColor(jni::JNIEnv& env, jni::String property) {
    auto color = Color::parse(jni::Make<std::string>(env, property));
    if (color) {
        light.setColor(color.value());
    }
}

jni::String Light::getColor(jni::JNIEnv &env) {
    auto color = light.getColor().asConstant();
    return jni::Make<jni::String>(env, color.stringify());
}

jni::Object<TransitionOptions> Light::getColorTransition(jni::JNIEnv& env) {
   using namespace mbgl::android::conversion;
   mbgl::style::TransitionOptions options = light.getColorTransition();
   return *convert<jni::Object<TransitionOptions>>(env, options);
}

void Light::setColorTransition(jni::JNIEnv&, jlong duration, jlong delay) {
   mbgl::style::TransitionOptions options;
   options.duration.emplace(mbgl::Milliseconds(duration));
   options.delay.emplace(mbgl::Milliseconds(delay));
   light.setColorTransition(options);
}

void Light::setIntensity(jni::JNIEnv&, jni::jfloat property) {
    light.setIntensity(property);
}

jni::jfloat Light::getIntensity(jni::JNIEnv&) {
    return light.getIntensity().asConstant();
}

jni::Object<TransitionOptions> Light::getIntensityTransition(jni::JNIEnv& env) {
   using namespace mbgl::android::conversion;
   mbgl::style::TransitionOptions options = light.getIntensityTransition();
   return *convert<jni::Object<TransitionOptions>>(env, options);
}

void Light::setIntensityTransition(jni::JNIEnv&, jlong duration, jlong delay) {
   mbgl::style::TransitionOptions options;
   options.duration.emplace(mbgl::Milliseconds(duration));
   options.delay.emplace(mbgl::Milliseconds(delay));
   light.setIntensityTransition(options);
}

void Light::registerNative(jni::JNIEnv& env) {
    // Lookup the class
    Light::javaClass = *jni::Class<Light>::Find(env).NewGlobalRef(env).release();

#define METHOD(MethodPtr, name) jni::MakeNativePeerMethod<decltype(MethodPtr), (MethodPtr)>(name)
    // Register the peer
    jni::RegisterNativePeer<Light>(env, Light::javaClass, "nativePtr",
    METHOD(&Light::getAnchor, "nativeGetAnchor"),
    METHOD(&Light::setAnchor, "nativeSetAnchor"),
    METHOD(&Light::getPositionTransition, "nativeGetPositionTransition"),
    METHOD(&Light::setPositionTransition, "nativeSetPositionTransition"),
    METHOD(&Light::getPosition, "nativeGetPosition"),
    METHOD(&Light::setPosition, "nativeSetPosition"),
    METHOD(&Light::getColorTransition, "nativeGetColorTransition"),
    METHOD(&Light::setColorTransition, "nativeSetColorTransition"),
    METHOD(&Light::getColor, "nativeGetColor"),
    METHOD(&Light::setColor, "nativeSetColor"),
    METHOD(&Light::getIntensityTransition, "nativeGetIntensityTransition"),
    METHOD(&Light::setIntensityTransition, "nativeSetIntensityTransition"),
    METHOD(&Light::getIntensity, "nativeGetIntensity"),
    METHOD(&Light::setIntensity, "nativeSetIntensity"));
}

} // namespace android
} // namespace mb
