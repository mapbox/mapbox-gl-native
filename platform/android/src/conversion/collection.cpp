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

}
}
}
