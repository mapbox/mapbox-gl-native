#include "sources.hpp"

#include "../value.hpp"
#include "../android_conversion.hpp"
#include "../android_geojson.hpp"

#include <mbgl/util/constants.hpp>
#include <mbgl/style/conversion.hpp>
#include <mbgl/style/conversion/source.hpp>

#include <string>

namespace mbgl {
namespace android {

    mbgl::optional<std::unique_ptr<mbgl::style::Source>> convertToNativeSource(jni::JNIEnv& env, jni::Object<jni::jobject> jvalue, jni::String id) {
        using namespace mbgl::style;

        Value value(env, jvalue);
        conversion::Result<std::unique_ptr<Source>> source = conversion::convert<std::unique_ptr<Source>>(value, jni::Make<std::string>(env, id));
        if (!source) {
            mbgl::Log::Error(mbgl::Event::JNI, "Unable to add source: " + source.error().message);
            return {};
        }
        return std::move(*source);
    }
}
}