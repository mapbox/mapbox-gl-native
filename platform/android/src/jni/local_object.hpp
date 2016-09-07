#pragma once

#include <jni/jni.hpp>

namespace jni {

    class LocalRefDeleter {
        private:
            JNIEnv* env = nullptr;

        public:
            LocalRefDeleter() = default;
            LocalRefDeleter(JNIEnv& e) : env(&e) {}

            void operator()(jobject* object) const {
                if (object) {
                    assert(env);
                    DeleteLocalRef(*env, object);
                }
            }
    };

    template < class T >
    using LocalObject = std::unique_ptr< T, LocalRefDeleter >;

    /**
     * Use a LocalObject to discard of local references as soon as possible
     */
    template < class T >
    LocalObject<T> NewLocalObject(JNIEnv& env, T* t) {
        return LocalObject<T>(t, LocalRefDeleter(env));
    }
}
