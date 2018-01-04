#pragma once

#include <jni/jni.hpp>

#include "../attach_env.hpp"

namespace mbgl {
namespace android {

// A deleter that doesn't retain an JNIEnv handle but instead tries to attach the JVM. This means
// it can be used on any thread to delete a global ref.
struct GenericGlobalRefDeleter {
    void operator()(jni::jobject* p) const {
        if (p) {
            auto env = AttachEnv();
            env->DeleteGlobalRef(jni::Unwrap(p));
        }
    }
};


template < class TagType >
class GenericWeakObjectRefDeleter;

template < class TagType = jni::ObjectTag >
using GenericUniqueWeakObject = std::unique_ptr< const jni::Object<TagType>, GenericWeakObjectRefDeleter<TagType> >;

template < class TagType >
class GenericWeakObjectRefDeleter
{
public:
    using pointer = jni::PointerToValue< jni::Object<TagType> >;

    void operator()(pointer p) const
    {
        if (p)
        {
            auto env = AttachEnv();
            env->DeleteWeakGlobalRef(jni::Unwrap(p->Get()));
        }
    }
};

template < class TagType >
GenericUniqueWeakObject<TagType> SeizeGenericWeakRef(JNIEnv&, jni::Object<TagType>&& object)
{
    return GenericUniqueWeakObject<TagType>(jni::PointerToValue<jni::Object<TagType>>(std::move(object)), GenericWeakObjectRefDeleter<TagType>());
};

} // namespace android
} // namespace mbgl
