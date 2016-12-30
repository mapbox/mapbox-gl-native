#pragma once

// GCC 4.9 compatibility
#if !defined(__GNUC__) || __GNUC__ >= 5
#define GEOMETRYHPP_CONSTEXPR constexpr
#else
#define GEOMETRYHPP_CONSTEXPR inline
#endif

namespace mapbox {
namespace geometry {

template <typename T>
GEOMETRYHPP_CONSTEXPR point<T> operator+(point<T> const& lhs, point<T> const& rhs)
{
    return point<T>(lhs.x + rhs.x, lhs.y + rhs.y);
}

template <typename T>
GEOMETRYHPP_CONSTEXPR point<T> operator+(point<T> const& lhs, T const& rhs)
{
    return point<T>(lhs.x + rhs, lhs.y + rhs);
}

template <typename T>
GEOMETRYHPP_CONSTEXPR point<T> operator-(point<T> const& lhs, point<T> const& rhs)
{
    return point<T>(lhs.x - rhs.x, lhs.y - rhs.y);
}

template <typename T>
GEOMETRYHPP_CONSTEXPR point<T> operator-(point<T> const& lhs, T const& rhs)
{
    return point<T>(lhs.x - rhs, lhs.y - rhs);
}

template <typename T>
GEOMETRYHPP_CONSTEXPR point<T> operator*(point<T> const& lhs, point<T> const& rhs)
{
    return point<T>(lhs.x * rhs.x, lhs.y * rhs.y);
}

template <typename T>
GEOMETRYHPP_CONSTEXPR point<T> operator*(point<T> const& lhs, T const& rhs)
{
    return point<T>(lhs.x * rhs, lhs.y * rhs);
}

template <typename T>
GEOMETRYHPP_CONSTEXPR point<T> operator/(point<T> const& lhs, point<T> const& rhs)
{
    return point<T>(lhs.x / rhs.x, lhs.y / rhs.y);
}

template <typename T>
GEOMETRYHPP_CONSTEXPR point<T> operator/(point<T> const& lhs, T const& rhs)
{
    return point<T>(lhs.x / rhs, lhs.y / rhs);
}

template <typename T>
GEOMETRYHPP_CONSTEXPR point<T>& operator+=(point<T>& lhs, point<T> const& rhs)
{
    lhs.x += rhs.x;
    lhs.y += rhs.y;
    return lhs;
}

template <typename T>
GEOMETRYHPP_CONSTEXPR point<T>& operator+=(point<T>& lhs, T const& rhs)
{
    lhs.x += rhs;
    lhs.y += rhs;
    return lhs;
}

template <typename T>
GEOMETRYHPP_CONSTEXPR point<T>& operator-=(point<T>& lhs, point<T> const& rhs)
{
    lhs.x -= rhs.x;
    lhs.y -= rhs.y;
    return lhs;
}

template <typename T>
GEOMETRYHPP_CONSTEXPR point<T>& operator-=(point<T>& lhs, T const& rhs)
{
    lhs.x -= rhs;
    lhs.y -= rhs;
    return lhs;
}

template <typename T>
GEOMETRYHPP_CONSTEXPR point<T>& operator*=(point<T>& lhs, point<T> const& rhs)
{
    lhs.x *= rhs.x;
    lhs.y *= rhs.y;
    return lhs;
}

template <typename T>
GEOMETRYHPP_CONSTEXPR point<T>& operator*=(point<T>& lhs, T const& rhs)
{
    lhs.x *= rhs;
    lhs.y *= rhs;
    return lhs;
}

template <typename T>
GEOMETRYHPP_CONSTEXPR point<T>& operator/=(point<T>& lhs, point<T> const& rhs)
{
    lhs.x /= rhs.x;
    lhs.y /= rhs.y;
    return lhs;
}

template <typename T>
GEOMETRYHPP_CONSTEXPR point<T>& operator/=(point<T>& lhs, T const& rhs)
{
    lhs.x /= rhs;
    lhs.y /= rhs;
    return lhs;
}

} // namespace geometry
} // namespace mapbox
