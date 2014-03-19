#ifndef LLMR_UTIL_RECT
#define LLMR_UTIL_RECT

namespace llmr {

template <typename T>
struct Rect {
    explicit Rect(T x, T y, T w, T h) : x(x), y(y), w(w), h(h) {}
    T x = 0, y = 0;
    T w = 0, h = 0;

    operator bool() const { return w == 0 && h == 0; }
};
}

#endif
