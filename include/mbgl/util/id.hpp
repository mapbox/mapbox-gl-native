#ifndef MBGL_UTIL_ID
#define MBGL_UTIL_ID

#include <cstdint>
#include <string>
#include <iosfwd>

namespace mbgl {
namespace util {

class Dictionary {
public:
    using Type = uint32_t;

public:
    static Type Lookup(const std::string&);
    static const std::string& Lookup(Type);
};

template <typename Tag>
class ID {
public:
    static constexpr Dictionary::Type None = 0;

    ID() : id(None) {
    }
    ID(const std::string& name) : id(Dictionary::Lookup(name)) {
    }

    ID& operator=(const ID& rhs) = default;
    ID& operator=(const std::string& name) {
        id = Dictionary::Lookup(name);
        return *this;
    }

    bool valid() const {
        return id != None;
    }
    bool operator==(const ID& rhs) const {
        return valid() && id == rhs.id;
    }
    bool operator!=(const ID& rhs) const {
        return !valid() || id != rhs.id;
    }
    bool operator<(const ID& rhs) const {
        return id < rhs.id;
    }

    const std::string& str() const {
        return Dictionary::Lookup(id);
    }
    const char* c_str() const {
        return str().c_str();
    }

private:
    Dictionary::Type id;

    friend struct ::std::hash<ID>;
};

template <typename Tag>
inline std::ostream& operator<<(std::ostream& os, ID<Tag> t) {
    return os << t.str();
}

} // namespace util
} // namespace mbgl

namespace std {

template <typename Tag>
struct hash<mbgl::util::ID<Tag>> {
    typedef mbgl::util::ID<Tag> argument_type;
    typedef std::size_t result_type;

    result_type operator()(mbgl::util::ID<Tag> id) const {
        return static_cast<result_type>(id.id);
    }
};

} // namespace std

#endif
