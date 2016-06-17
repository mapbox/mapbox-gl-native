#pragma once

namespace mbgl {

template <typename T>
struct Rect {
    Rect() = default;
    Rect(T x_, T y_, T w_, T h_) : x(x_), y(y_), w(w_), h(h_) {}
    T x = 0, y = 0;
    T w = 0, h = 0;

    template <typename Number>
    Rect operator *(Number value) const {
        return Rect(x * value, y * value, w * value, h * value);
    }

    template <typename R>
    bool operator==(const R& r) const {
        return x == r.x && y == r.y && w == r.w && h == r.h;
    }

    bool hasArea() const { return w != 0 && h != 0; }
};
} // namespace mbgl
