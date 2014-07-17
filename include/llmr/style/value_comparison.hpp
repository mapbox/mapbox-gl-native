#ifndef LLMR_STYLE_VALUE_COMPARISON
#define LLMR_STYLE_VALUE_COMPARISON

#include "value.hpp"
#include <cstdlib>
#include <cerrno>

namespace llmr {

namespace util {

namespace detail {

template <typename Operator>
struct relaxed_operator_visitor {
    typedef bool result_type;

    inline bool operator()(bool lhs, bool rhs) const { return Operator()(lhs, rhs); }

    template <typename T, class = typename std::enable_if<std::is_arithmetic<T>::value>::type>
    inline bool operator()(bool lhs, T rhs) const { return Operator()(T(lhs), rhs); }

    template <typename T, class = typename std::enable_if<std::is_arithmetic<T>::value>::type>
    inline bool operator()(T lhs, bool rhs) const { return Operator()(lhs, T(rhs)); }

    inline bool operator()(int64_t lhs, uint64_t rhs) const {
        return lhs < 0 ? false : Operator()(uint64_t(lhs), rhs);
    }
    inline bool operator()(uint64_t lhs, int64_t rhs) const {
        return rhs < 0 ? false : Operator()(lhs, uint64_t(rhs));
    }

    template <typename T, class = typename std::enable_if<std::is_arithmetic<T>::value>::type>
    inline bool operator()(const std::string &lhs, T rhs) const {
        double value;
        return parseNumericString(lhs, value) ? Operator()(value, double(rhs)) : false;
    }

    template <typename T, class = typename std::enable_if<std::is_arithmetic<T>::value>::type>
    inline bool operator()(T lhs, const std::string &rhs) const {
        double value;
        return parseNumericString(rhs, value) ? Operator()(double(lhs), value) : false;
    }

    template <typename T0, typename T1>
    inline bool operator()(T0 lhs, T1 rhs) const { return Operator()(lhs, rhs); }
};

struct relaxed_equal_operator {
    template <typename T0, typename T1>
    inline bool operator()(T0 lhs, T1 rhs) const { return lhs == rhs; }
};

struct relaxed_greater_operator {
    template <typename T0, typename T1>
    inline bool operator()(T0 lhs, T1 rhs) const { return lhs > rhs; }
};

struct relaxed_greater_equal_operator {
    template <typename T0, typename T1>
    inline bool operator()(T0 lhs, T1 rhs) const { return lhs >= rhs; }
};

struct relaxed_less_operator {
    template <typename T0, typename T1>
    inline bool operator()(T0 lhs, T1 rhs) const { return lhs < rhs; }
};

struct relaxed_less_equal_operator {
    template <typename T0, typename T1>
    inline bool operator()(T0 lhs, T1 rhs) const { return lhs <= rhs; }
};

} // end namespace detail

inline bool relaxed_equal(Value const &lhs, Value const &rhs) {
    return apply_visitor(detail::relaxed_operator_visitor<detail::relaxed_equal_operator>(), lhs, rhs);
}

inline bool relaxed_greater(Value const &lhs, Value const &rhs) {
    return apply_visitor(detail::relaxed_operator_visitor<detail::relaxed_greater_operator>(), lhs, rhs);
}

inline bool relaxed_greater_equal(Value const &lhs, Value const &rhs) {
    return apply_visitor(detail::relaxed_operator_visitor<detail::relaxed_greater_equal_operator>(), lhs, rhs);
}

inline bool relaxed_less(Value const &lhs, Value const &rhs) {
    return apply_visitor(detail::relaxed_operator_visitor<detail::relaxed_less_operator>(), lhs, rhs);
}

inline bool relaxed_less_equal(Value const &lhs, Value const &rhs) {
    return apply_visitor(detail::relaxed_operator_visitor<detail::relaxed_less_equal_operator>(), lhs, rhs);
}

}

}

#endif
