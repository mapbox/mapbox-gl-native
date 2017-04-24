#pragma once

#include <mbgl/util/optional.hpp>

namespace mbgl {
namespace style {

template <class T>
class PaintPropertyStatistics {
public:
    optional<T> max() const { return {}; }
    void add(const T&) {}
};

template <>
class PaintPropertyStatistics<float> {
public:
    optional<float> max() const {
        return _max;
    }

    void add(float value) {
        _max = _max ? std::max(*_max, value) : value;
    }

private:
    optional<float> _max;
};

} // namespace style
} // namespace mbgl
