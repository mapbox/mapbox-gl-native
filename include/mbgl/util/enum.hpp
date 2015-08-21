#ifndef MBGL_UTIL_ENUM
#define MBGL_UTIL_ENUM

#include <iosfwd>
#include <string>

namespace mbgl {

template <typename Type>
struct EnumValue {
    const Type value;
    const char *name;
};

template <typename EnumName, const EnumValue<EnumName> *names, const size_t length>
struct Enum {
    using Type = EnumName;
    Type value;
    static const constexpr size_t l = length;
private:
    static constexpr inline bool compare(const char *a, const char *b) {
        return *a == *b && (*a == '\0' || compare(a + 1, b + 1));
    }
    static constexpr inline const char *lookup_type(Type e, EnumValue<Type> const * const list, size_t r) {
        return r == 0 ? "" : list->value == e ? list->name : lookup_type(e, list + 1, r - 1);
    }
    static constexpr inline Type lookup_name(const char *n, EnumValue<Type> const * const list, size_t r) {
        return r == 0 ? Type(-1) : compare(list->name, n) ? list->value : lookup_name(n, list + 1, r - 1);
    }
public:
    inline constexpr Enum(const char *n) : value(lookup_name(n, names, length)) {}
    inline constexpr Enum(const std::string &n) : value(lookup_name(n.c_str(), names, length)) {}
    inline constexpr Enum(Type t) : value(t) {}

    inline void operator=(const char *n) { value = lookup_name(n, names, length); }
    inline void operator=(const std::string &n) { *this = n.c_str(); }
    inline void operator=(Type t) { value = t; }

    inline constexpr bool valid() const { return value != Type(-1); }

    inline constexpr const char *c_str() const { return lookup_type(value, names, length); }
    inline std::string str() const { return c_str(); }

    inline constexpr operator Type() const { return value; }
};

#define MBGL_DEFINE_ENUM_CLASS(name, type, strings...) \
    const constexpr ::mbgl::EnumValue<type> type##_names[] = strings; \
    using name = ::mbgl::Enum<type, type##_names, sizeof(type##_names) / sizeof(::mbgl::EnumValue<type>)>; \
    inline std::ostream& operator<<(std::ostream& os, type t) { return os << name(t).str(); }

}

#endif

