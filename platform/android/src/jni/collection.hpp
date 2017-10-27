#pragma once

#include <jni/jni.hpp>

#include <string>
#include <vector>

namespace jni {

inline Array<String> MakeAnything(ThingToMake<Array<String>>, JNIEnv& env, const std::vector<std::string>& vector)
{
    static auto clazz = *Class<StringTag>::Find(env).NewGlobalRef(env).release();
    auto result = Array<String>::New(env, vector.size(), clazz);

    std::size_t index = 0;
    for (auto&& item : vector) {
        auto element = Make<jni::String>(env, item);
        result.Set(env, index, element);
        DeleteLocalRef(env, element);
        index++;
    }

    return result;
}

}
