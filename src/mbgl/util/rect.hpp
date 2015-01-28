#ifndef MBGL_UTIL_RECT
#define MBGL_UTIL_RECT

namespace mbgl {

template <typename T>
struct Rect {
    inline Rect() {}
    inline Rect(T x_, T y_, T w_, T h_) : x(x_), y(y_), w(w_), h(h_) {}
    T x = 0, y = 0;
    T w = 0, h = 0;
    T originalW = 0, originalH = 0;

    template <typename Number>
    Rect operator *(Number value) const {
        return Rect(x * value, y * value, w * value, h * value);
    }

    operator bool() const { return w != 0 && h != 0; }
};
}

#endif
