#ifndef LLMR_STYLE_VALUE
#define LLMR_STYLE_VALUE

#include <llmr/util/variant.hpp>
#include <llmr/util/pbf.hpp>

namespace llmr {

typedef util::variant<bool, int64_t, uint64_t, double, std::string> Value;

std::string toString(const Value &value);

Value parseValue(pbf data);

namespace util {

template <typename T>
struct string_to_number {};

template <> struct string_to_number<bool> {
    bool operator()(std::string str) const {
        std::transform(str.begin(), str.end(), str.begin(), ::tolower);
        str.erase(std::remove_if(str.begin(), str.end(), ::isspace), str.end());
        return !(str == "false" || str == "null" || !str.length());
    }
};

template <> struct string_to_number<double> {
    double operator()(std::string const &str) const try { return std::stod(str); }
    catch (...) { return string_to_number<bool>()(str); }
};

template <> struct string_to_number<int64_t> {
    int64_t operator()(std::string const &str) const try { return std::stoll(str); }
    catch (...) { return string_to_number<bool>()(str); }
};

template <> struct string_to_number<uint64_t> {
    uint64_t operator()(std::string const &str) const try { return std::stoull(str); }
    catch (...) { return string_to_number<bool>()(str); }
};

struct relaxed_equal_visitor {
    bool operator()(bool lhs, bool rhs) const { return lhs == rhs; }

    template <typename T, class = typename std::enable_if<std::is_arithmetic<T>::value>::type>
    bool operator()(bool lhs, T rhs) const { return lhs == bool(rhs); }

    template <typename T, class = typename std::enable_if<std::is_arithmetic<T>::value>::type>
    bool operator()(T lhs, bool rhs) const { return bool(lhs) == rhs; }

    bool operator()(int64_t lhs, uint64_t rhs) const { return lhs < 0 ? false : uint64_t(lhs) == rhs; }
    bool operator()(uint64_t lhs, int64_t rhs) const { return rhs < 0 ? false : lhs == uint64_t(rhs); }

    template <typename T, class = typename std::enable_if<std::is_arithmetic<T>::value>::type>
    bool operator()(T lhs, const std::string &rhs) const { return lhs == string_to_number<T>()(rhs); }

    template <typename T, class = typename std::enable_if<std::is_arithmetic<T>::value>::type>
    bool operator()(const std::string &lhs, T rhs) const { return string_to_number<T>()(lhs) == rhs; }

    template <typename T0, typename T1>
    bool operator()(T0 lhs, T1 rhs) const { return lhs == rhs; }
};

template <typename T>
struct relaxed_equal {
    relaxed_equal(T const &lhs) : lhs(lhs) {}

    bool operator()(T const &rhs) const {
        return apply_visitor(lhs, rhs, relaxed_equal_visitor());
    }
    T const &lhs;
};
}
}

#endif
