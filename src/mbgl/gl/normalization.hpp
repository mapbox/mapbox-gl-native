#pragma once

#include <mbgl/math/clamp.hpp>

#include <cassert>
#include <limits>

namespace mbgl {
namespace gl {

template <class T>
class Normalized {
public:
    T value;

    Normalized() : value(0) {}

    explicit Normalized(float f)
        : value(static_cast<T>(std::numeric_limits<T>::max() * util::clamp(f, 0.0f, 1.0f))) {
        assert(f >= 0.0f);
        assert(f <= 1.0f);
    }

    float denormalized() const {
        return float(value) / std::numeric_limits<T>::max();
    }
};

template <class T>
bool operator==(const Normalized<T>& lhs, const Normalized<T>& rhs) {
    return lhs.value == rhs.value;
}

} // namespace gl
} // namespace mbgl
