#ifndef MBGL_UTIL_STRING
#define MBGL_UTIL_STRING

#include <string>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunknown-pragmas"
#pragma GCC diagnostic ignored "-Wunused-local-typedefs"
#include <boost/lexical_cast.hpp>
#pragma GCC diagnostic pop

namespace mbgl {
namespace util {

template <typename... Args>
inline std::string toString(Args&&... args) {
    return boost::lexical_cast<std::string>(::std::forward<Args>(args)...);
}

// boost::lexical_cast() treats this as a character, but we are using it as number types.
inline std::string toString(int8_t num) {
    return boost::lexical_cast<std::string>(int(num));
}

// boost::lexical_cast() treats this as a character, but we are using it as number types.
inline std::string toString(uint8_t num) {
    return boost::lexical_cast<std::string>(unsigned(num));
}

inline std::string toString(std::exception_ptr error) {
    assert(error);

    if (!error) {
        return "(null)";
    }

    try {
        std::rethrow_exception(error);
    } catch (const std::exception& ex) {
        return ex.what();
    } catch (...) {
        return "Unknown exception type";
    }
}

template<size_t max, typename... Args>
inline std::string sprintf(const char *msg, Args... args) {
    char res[max];
    int len = snprintf(res, sizeof(res), msg, args...);
    return std::string(res, len);
}

template<size_t max, typename... Args>
inline std::string sprintf(const std::string &msg, Args... args) {
    return sprintf<max>(msg.c_str(), args...);
}

} // namespace util
} // namespace mbgl

#endif
