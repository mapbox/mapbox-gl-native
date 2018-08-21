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
        jni::String jstr = array.Get(env, i);
        vector.push_back(*convert<std::string, jni::String>(env, jstr));
        jni::DeleteLocalRef(env, jstr);
    }

    return vector;
}

jni::Array<jni::String> toArray(JNIEnv& env, const std::vector<std::string>& vector) {
    static auto clazz = *jni::Class<jni::StringTag>::Find(env).NewGlobalRef(env).release();
    auto result = jni::Array<jni::String>::New(env, vector.size(), clazz);

    std::size_t index = 0;
    for (auto&& item : vector) {
        auto element = jni::Make<jni::String>(env, item);
        result.Set(env, index, element);
        DeleteLocalRef(env, element);
        index++;
    }

    return result;
}

}
}
}
