#ifndef LLMR_STYLE_VALUE
#define LLMR_STYLE_VALUE

#include <llmr/util/variant.hpp>
#include <llmr/util/pbf.hpp>

namespace llmr {

typedef util::variant<bool, int64_t, uint64_t, double, std::string> Value;

std::string toString(const Value &value);

Value parseValue(pbf data);

namespace util {

inline int string_to_bool(std::string str) {
    std::transform(str.begin(), str.end(), str.begin(), ::tolower);
    if (str == "false" || str == "null" || !str.length()) return 0;
    else if (str == "true") return 1;
    else return -1;
}

template <typename T>
struct string_to_number {};

template <> struct string_to_number<bool> {
    bool operator()(const std::string &str) const {
        // Converts 0 => false, 1 => true and -1 => true.
        return string_to_bool(str);
    }
};

template <> struct string_to_number<double> {
    double operator()(std::string const &str) const {
        int val = string_to_bool(str);
        if (val < 0) {
            try {
                double b = std::stod(str);
                return b;
            } catch (std::invalid_argument const& ex) {
                return false;
            }
        } else {
            return val;
        }
        return false;
    }
};

template <> struct string_to_number<int64_t> {
    int64_t operator()(std::string const &str) const {
        int val = string_to_bool(str);
        if (val < 0) {
            try {
                long long b = std::stoll(str);
                return b;
            } catch (std::invalid_argument const& ex) {
                return false;
            }
        } else {
            return val;
        }
        return false;
    }
};

template <> struct string_to_number<uint64_t> {
    uint64_t operator()(std::string const &str) const {
        int val = string_to_bool(str);
        if (val < 0) {
            try {
                unsigned long long b = std::stoull(str);
                return b;
            } catch (std::invalid_argument const& ex) {
                return false;
            }
        } else {
            return val;
        }
        return false;
    }
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
    bool operator()(T lhs, const std::string &rhs) const try { return lhs == string_to_number<T>()(rhs); }
    catch(...) { return false; }

    template <typename T, class = typename std::enable_if<std::is_arithmetic<T>::value>::type>
    bool operator()(const std::string &lhs, T rhs) const try { return string_to_number<T>()(lhs) == rhs; }
    catch(...) { return false; }

    template <typename T0, typename T1>
    bool operator()(T0 lhs, T1 rhs) const { return lhs == rhs; }
};

struct relaxed_equal {
    relaxed_equal(Value const &lhs) : lhs(lhs) {}

    bool operator()(Value const &rhs) const {
        return apply_visitor(lhs, rhs, relaxed_equal_visitor());
    }
    Value const &lhs;
};
}
}

#endif
