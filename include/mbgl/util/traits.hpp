#pragma once

#include <cstdint>
#include <type_traits>

namespace mbgl {

template<typename T>
constexpr auto underlying_type(T t) -> typename std::underlying_type_t<T> {
    return typename std::underlying_type_t<T>(t);
}

template <typename T> struct is_utf16char_like: std::false_type {};
template <typename T> struct is_utf16char_like<const T>: is_utf16char_like<T> {};
template <> struct is_utf16char_like<char16_t>: std::true_type {};
template <> struct is_utf16char_like<wchar_t>: std::true_type {};
template <> struct is_utf16char_like<uint16_t>: std::true_type {};

template <typename T> using is_utf16char_like_pointer = std::integral_constant<bool, std::is_pointer<T>::value
                                                                                     && is_utf16char_like<typename std::remove_pointer<T>::type>::value>;

template <class OutPointer, class InChar>
typename std::enable_if<is_utf16char_like<InChar>::value && is_utf16char_like_pointer<OutPointer>::value, OutPointer>::type utf16char_cast(InChar *in)
{
    return reinterpret_cast<OutPointer>(in);
}

} // namespace mbgl
