#pragma once

#include <mbgl/util/variant.hpp>

#include <jni/jni.hpp>

#include <string>

namespace mbgl {
namespace android {
namespace conversion {

struct Error { std::string message; };

template <class T>
class Result : private variant<T, Error> {
public:
    using variant<T, Error>::variant;

    explicit operator bool() const {
        return this->template is<T>();
    }

    T& operator*() {
        assert(this->template is<T>());
        return this->template get<T>();
    }

    const T& operator*() const {
        assert(this->template is<T>());
        return this->template get<T>();
    }

    const Error& error() const {
        assert(this->template is<Error>());
        return this->template get<Error>();
    }
};

template <class T, class V, class Enable = void>
struct Converter;

template <class T, typename V, class...Args>
Result<T> convert(jni::JNIEnv& env, const V& value, Args&&...args) {
    return Converter<T, V>()(env, value, std::forward<Args>(args)...);
}

} // namespace conversion
} // namespace android
} // namespace mbgl
