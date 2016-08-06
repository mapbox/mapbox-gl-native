#pragma once

namespace mbgl {
namespace gl {

template <typename T>
class Preserve {
public:
    Preserve() : data(T::Get()) {
    }
    ~Preserve() {
        T::Set(data);
    }

private:
    const typename T::Type data;
};

} // namespace gl
} // namespace mbgl
