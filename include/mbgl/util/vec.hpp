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
    struct null {};
    typedef T Type;

    T x, y;

    inline vec2() {}

    template<typename U = T, typename std::enable_if<std::numeric_limits<U>::has_quiet_NaN, int>::type = 0>
    inline vec2(null) : x(std::numeric_limits<T>::quiet_NaN()), y(std::numeric_limits<T>::quiet_NaN()) {}

    template<typename U = T, typename std::enable_if<!std::numeric_limits<U>::has_quiet_NaN, int>::type = 0>
    inline vec2(null) : x(std::numeric_limits<T>::min()), y(std::numeric_limits<T>::min()) {}

    inline vec2(const vec2& o) : x(o.x), y(o.y) {}

    template<typename U>
    inline vec2(const U& u) : x(u.x), y(u.y) {}

    inline vec2(T x, T y) : x(x), y(y) {}

    inline bool operator==(const vec2& rhs) const {
        return x == rhs.x && y == rhs.y;
    }

    template <typename O>
    inline typename std::enable_if<std::is_arithmetic<O>::value, vec2>::type
    operator*(O o) const {
        return {x * o, y * o};
    }

    template <typename O>
    inline typename std::enable_if<std::is_arithmetic<O>::value, vec2>::type &
    operator*=(O o) {
        x *= o;
        y *= o;
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
        return {x - o.x, y - o.y};
    }

    template <typename M>
    inline vec2 matMul(const M &m) const {
        return {m[0] * x + m[1] * y, m[2] * x + m[3] * y};
    }

    template<typename U = T, typename std::enable_if<std::numeric_limits<U>::has_quiet_NaN, int>::type = 0>
    inline operator bool() const {
        return !isnan(x) && !isnan(y);
    }

    template<typename U = T, typename std::enable_if<!std::numeric_limits<U>::has_quiet_NaN, int>::type = 0>
    inline operator bool() const {
        return x != std::numeric_limits<T>::min() && y != std::numeric_limits<T>::min();
    }
};

template <typename T = double>
struct vec3 {
    T x, y, z;

    inline vec3() {}
    inline vec3(const vec3& o) : x(o.x), y(o.y), z(o.z) {}
    inline vec3(T x, T y, T z) : x(x), y(y), z(z) {}
    inline bool operator==(const vec3& rhs) const {
        return x == rhs.x && y == rhs.y && z == rhs.z;
    }
};

template <typename T = double>
struct vec4 {
    T x, y, z, w;

    inline vec4() {}
    inline vec4(const vec4& o) : x(o.x), y(o.y), z(o.z), w(o.w) {}
    inline vec4(T x, T y, T z, T w) : x(x), y(y), z(z), w(w) {}
    inline bool operator==(const vec4& rhs) const {
        return x == rhs.x && y == rhs.y && z == rhs.z && w == rhs.w;
    }
};


struct box {
    vec2<double> tl, tr, bl, br;
    vec2<double> center;
};

typedef vec2<int16_t> Coordinate;

}

#endif
