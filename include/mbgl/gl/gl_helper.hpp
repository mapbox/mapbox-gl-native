#pragma once

namespace mbgl {
namespace gl {

template <typename T>
class Preserve {
public:
    inline Preserve() : data(T::Get()) {
    }
    inline ~Preserve() {
        T::Set(data);
    }

private:
    const typename T::Type data;
};

} // namespace gl
} // namespace mbgl
