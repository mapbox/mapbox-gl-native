#pragma once

#include "../../conversion/conversion.hpp"

#include <jni/jni.hpp>
#include <mbgl/style/transition_options.hpp>
#include "../../jni/local_object.hpp"
#include "../transition_options.hpp"

namespace mbgl {
namespace android {
namespace conversion {

template<>
struct Converter<jni::Object<TransitionOptions>, mbgl::style::TransitionOptions> {
    Result<jni::Object<TransitionOptions>> operator()(jni::JNIEnv &env, const mbgl::style::TransitionOptions &value) const {

        // Convert duration
        jlong duration =  std::chrono::duration_cast<std::chrono::milliseconds>(value.duration.value_or(mbgl::Duration::zero())).count();
        // Convert delay
        jlong delay = std::chrono::duration_cast<std::chrono::milliseconds>(value.delay.value_or(mbgl::Duration::zero())).count();

        // Create transition options
        return TransitionOptions::fromTransitionOptions(env, duration, delay);
    }
};

}
}
}