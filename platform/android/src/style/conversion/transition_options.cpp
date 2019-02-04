#include "transition_options.hpp"

namespace mbgl {
namespace android {
namespace conversion {

Result<jni::Local<jni::Object<TransitionOptions>>> Converter<jni::Local<jni::Object<TransitionOptions>>, mbgl::style::TransitionOptions>::operator()(jni::JNIEnv& env, const mbgl::style::TransitionOptions& value) const {
    return TransitionOptions::fromTransitionOptions(env,
        std::chrono::duration_cast<std::chrono::milliseconds>(value.duration.value_or(mbgl::Duration::zero())).count(),
        std::chrono::duration_cast<std::chrono::milliseconds>(value.delay.value_or(mbgl::Duration::zero())).count(),
        (jboolean) value.enablePlacementTransitions
    );
}

}
}
}
