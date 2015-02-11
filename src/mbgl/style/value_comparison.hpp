#ifndef MBGL_STYLE_VALUE_COMPARISON
#define MBGL_STYLE_VALUE_COMPARISON

#include <mbgl/style/value.hpp>
#include <cstdlib>
#include <cerrno>

namespace mbgl {

namespace util {

namespace detail {

template <typename Operator>
struct relaxed_operator_visitor {
    typedef bool result_type;

    template <typename T0, typename T1>
    inline bool operator()(T0, T1) const { return false; }

    template <typename T>
    inline bool operator()(T lhs, T rhs) const { return Operator()(lhs, rhs); }

    inline bool operator()(int64_t lhs, uint64_t rhs) const {
        return Operator()(double(lhs), double(rhs));
    }

    inline bool operator()(int64_t lhs, double rhs) const {
        return Operator()(double(lhs), rhs);
    }

    inline bool operator()(uint64_t lhs, int64_t rhs) const {
        return Operator()(double(lhs), double(rhs));
    }

    inline bool operator()(uint64_t lhs, double rhs) const {
        return Operator()(double(lhs), rhs);
    }

    inline bool operator()(double lhs, uint64_t rhs) const {
        return Operator()(lhs, double(rhs));
    }

    inline bool operator()(double lhs, int64_t rhs) const {
        return Operator()(lhs, double(rhs));
    }
};

struct relaxed_equal_operator {
    template <typename T0, typename T1>
    inline bool operator()(T0 lhs, T1 rhs) const { return lhs == rhs; }
};

struct relaxed_not_equal_operator {
    template <typename T0, typename T1>
    inline bool operator()(T0 lhs, T1 rhs) const { return lhs != rhs; }
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

inline bool relaxed_not_equal(Value const &lhs, Value const &rhs) {
    return apply_visitor(detail::relaxed_operator_visitor<detail::relaxed_not_equal_operator>(), lhs, rhs);
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
