#ifndef MBGL_UTIL_RECT
#define MBGL_UTIL_RECT

namespace mbgl {

template <typename T>
struct Rect {
    explicit Rect(T x, T y, T w, T h) : x(x), y(y), w(w), h(h) {}
    T x = 0, y = 0;
    T w = 0, h = 0;

    template <typename Number>
    Rect operator *(Number value) const {
        return Rect(x * value, y * value, w * value, h * value);
    }

    operator bool() const { return w == 0 || h == 0; }
};
}

#endif
