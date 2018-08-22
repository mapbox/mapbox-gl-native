#pragma once

#include "../../conversion/conversion.hpp"
#include "../transition_options.hpp"

#include <mbgl/style/transition_options.hpp>

#include <jni/jni.hpp>

namespace mbgl {
namespace android {
namespace conversion {

template<>
struct Converter<jni::Local<jni::Object<TransitionOptions>>, mbgl::style::TransitionOptions> {
    Result<jni::Local<jni::Object<TransitionOptions>>> operator()(jni::JNIEnv&, const mbgl::style::TransitionOptions&) const;
};

}
}
}
