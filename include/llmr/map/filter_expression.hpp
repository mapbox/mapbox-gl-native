#ifndef LLMR_MAP_FILTER_EXPRESSION
#define LLMR_MAP_FILTER_EXPRESSION

#include <llmr/style/value.hpp>
#include <llmr/util/recursive_wrapper.hpp>

#include <vector>
#include <string>

namespace llmr {

enum class FilterOperator {
    Equal,
    NotEqual,
    Greater,
    GreaterEqual,
    Less,
    LessEqual
};

enum class ExpressionOperator {
    Or,
    And
};


inline FilterOperator filterOperatorType(const std::string &op) {
    if (op == "!=" || op == "not") {
        return FilterOperator::NotEqual;
    } else if (op == "==" || op == "eq") {
        return FilterOperator::Equal;
    } else if (op == ">" || op == "gt") {
        return FilterOperator::Greater;
    } else if (op == ">=" || op == "gte") {
        return FilterOperator::GreaterEqual;
    } else if (op == "<" || op == "lt") {
        return FilterOperator::Less;
    } else if (op == "<=" || op == "lte") {
        return FilterOperator::LessEqual;
    } else {
        fprintf(stderr, "[WARNING] filter operator '%s' unrecognized\n", op.c_str());
        return FilterOperator::Equal;
    }
}

inline ExpressionOperator expressionOperatorType(const std::string &op) {
    if (op == "&&" || op == "and") {
        return ExpressionOperator::And;
    } else if (op == "||" || op == "or") {
        return ExpressionOperator::Or;
    } else {
        fprintf(stderr, "[WARNING] expression operator '%s' unrecognized\n", op.c_str());
        return ExpressionOperator::Or;
    }
}


class PropertyFilter;
class PropertyExpression;

typedef util::variant<
    util::recursive_wrapper<PropertyFilter>,
    util::recursive_wrapper<PropertyExpression>,
    std::true_type
> PropertyFilterExpression;


class PropertyFilter {
public:
    inline PropertyFilter(const std::string &field, FilterOperator op, const Value &value) : field(field), op(op), value(value) {};
    inline PropertyFilter(PropertyFilter &&filter)
        : field(std::move(filter.field)),
          op(filter.op),
          value(std::move(filter.value)) {
    }

    // Returns true if the filter passes, even if the key is missing.
    inline bool isMissingFieldOkay() const {
        switch (op) {
        case FilterOperator::NotEqual:
            return true;
        default:
            return false;
        }
    }

    inline bool compare(const Value &other) const {
        switch (op) {
        case FilterOperator::Equal:
            return util::relaxed_equal(other, value);
        case FilterOperator::NotEqual:
            return !util::relaxed_equal(other, value);
        case FilterOperator::Greater:
            return util::relaxed_greater(other, value);
        case FilterOperator::GreaterEqual:
            return util::relaxed_greater_equal(other, value);
        case FilterOperator::Less:
            return util::relaxed_less(other, value);
        case FilterOperator::LessEqual:
            return util::relaxed_less_equal(other, value);
        default:
            return false;
        }
    }

public:
    std::string field;
    FilterOperator op = FilterOperator::Equal;
    Value value;
};

class PropertyExpression {
public:
    inline PropertyExpression() {}
    inline PropertyExpression(PropertyExpression &&expression)
        : op(expression.op),
          operands(std::move(expression.operands)) {
    }

public:
    ExpressionOperator op = ExpressionOperator::Or;
    std::vector<PropertyFilterExpression> operands;
};


}

#endif
