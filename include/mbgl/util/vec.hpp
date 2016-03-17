#ifndef MBGL_UTIL_VEC
#define MBGL_UTIL_VEC

#include <limits>
#include <type_traits>
#include <cmath>
#include <cstdint>
#include <array>

namespace mbgl {

template <typename T = double>
struct vec2 {
    typedef T Type;

    T x, y;

    inline vec2(const vec2& o) = default;

    template<typename U>
    inline vec2(const U& u) : x(u.x), y(u.y) {}

    inline vec2(T x_, T y_) : x(x_), y(y_) {}

    inline bool operator==(const vec2& rhs) const {
        return x == rhs.x && y == rhs.y;
    }

    template <typename O>
    inline typename std::enable_if<std::is_arithmetic<O>::value, vec2>::type
    operator*(O o) const {
        return {x * o, y * o};
    }

    inline void operator*=(T o) {
        x *= o;
        y *= o;
    }

    template <typename O>
    inline typename std::enable_if<std::is_arithmetic<O>::value, vec2>::type
    operator/(O o) const {
        return {x / o, y / o};
    }

    inline void operator/=(T o) {
        x /= o;
        y /= o;
    }

    inline vec2<T> operator *(const std::array<float, 16>& matrix) {
        return { x * matrix[0] + y * matrix[4] + matrix[12], x * matrix[1] + y * matrix[5] + matrix[13] };
    }

    template <typename O>
    inline typename std::enable_if<std::is_arithmetic<O>::value, vec2>::type
    operator-(O o) const {
        return {x - o, y - o};
    }

    template <typename O>
    inline typename std::enable_if<!std::is_arithmetic<O>::value, vec2>::type
    operator-(const O &o) const {
        return vec2<T>(x - o.x, y - o.y);
    }

    template <typename O>
    inline typename std::enable_if<!std::is_arithmetic<O>::value, vec2>::type
    operator+(const O &o) const {
        return vec2<T>(x + o.x, y + o.y);
    }

    template <typename M>
    inline vec2 matMul(const M &m) const {
        return {m[0] * x + m[1] * y, m[2] * x + m[3] * y};
    }
};

template <typename T = double>
struct vec4 {
    T x, y, z, w;

    inline vec4() = default;
    inline vec4(const vec4& o) : x(o.x), y(o.y), z(o.z), w(o.w) {}
    inline vec4(T x_, T y_, T z_, T w_) : x(x_), y(y_), z(z_), w(w_) {}
    inline bool operator==(const vec4& rhs) const {
        return x == rhs.x && y == rhs.y && z == rhs.z && w == rhs.w;
    }
};


} // namespace mbgl

#endif // MBGL_UTIL_VEC
