#include "collection.hpp"
#include "constant.hpp"

namespace mbgl {
namespace android {
namespace conversion {

std::vector<std::string> toVector(JNIEnv& env, jni::Array<jni::String> array) {
    std::size_t len = array.Length(env);
    std::vector<std::string> vector;
    vector.reserve(len);

    for (std::size_t i = 0; i < len; i++) {
        vector.push_back(*convert<std::string>(env, *jni::SeizeLocal(env, array.Get(env, i))));
    }

    return vector;
}

jni::Array<jni::String> toArray(JNIEnv& env, const std::vector<std::string>& vector) {
    auto result = jni::Array<jni::String>::New(env, vector.size());

    for (std::size_t i = 0; i < vector.size(); i++) {
        result.Set(env, i, *jni::SeizeLocal(env, jni::Make<jni::String>(env, vector.at(i))));
    }

    return result;
}

}
}
}
