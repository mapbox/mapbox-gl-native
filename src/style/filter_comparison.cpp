#include <llmr/map/vector_tile.hpp>
#include <llmr/style/filter_comparison_private.hpp>

#include <llmr/style/value_comparison.hpp>

#include <ostream>

namespace llmr {


inline bool includes(const Value &property_value, const std::vector<Value> &filter_values) {
    for (const Value &filter_value : filter_values) {
        if (util::relaxed_equal(property_value, filter_value)) {
            return true;
        }
    }
    return false;
}

template <typename Comparer>
inline bool compare(const Value &property_value, const std::vector<Value> &filter_values, const Comparer &comparer) {
    for (const Value &filter_value : filter_values) {
        if (!comparer(property_value, filter_value)) {
            return false;
        }
    }
    return true;
}

template <typename Comparer>
inline bool all(const std::vector<Value> &property_values, const std::vector<Value> &filter_values, const Comparer &comparer) {
    for (const Value &property_value : property_values) {
        if (!compare(property_value, filter_values, comparer)) {
            return false;
        }
    }
    return true;
}


inline bool set_equal(const std::vector<Value> &property_values, const std::vector<Value> &filter_values) {
    for (const Value &property_value : property_values) {
        if (!includes(property_value, filter_values)) {
            return false;
        }
    }
    if (property_values.size() == filter_values.size()) {
        // Optimization: When the count is the same, the set is guaranteed to be identical.
        return true;
    }
    // Otherwise, check again for identical reverse-mapped values.
    for (const Value &filter_value : filter_values) {
        if (!includes(filter_value, property_values)) {
            return false;
        }
    }
    return true;
}


bool FilterComparison::Instance::compare(const std::vector<Value> &property_values) const {
    switch (op) {
        case Operator::Equal:
            return set_equal(property_values, values);
        case Operator::NotEqual:
            return !set_equal(property_values, values);
        case Operator::In:
            for (const Value &property_value : property_values) {
                if (includes(property_value, values)) {
                    return true;
                }
            }
            return false;
        case Operator::NotIn:
            for (const Value &property_value : property_values) {
                if (!includes(property_value, values)) {
                    return true;
                }
            }
            return false;
        case Operator::Greater:
            return all(property_values, values, util::relaxed_greater);
        case Operator::GreaterEqual:
            return all(property_values, values, util::relaxed_greater_equal);
        case Operator::Less:
            return all(property_values, values, util::relaxed_less);
        case Operator::LessEqual:
            return all(property_values, values, util::relaxed_less_equal);
        default:
            return false;
    }
}


const std::string &FilterComparison::getField() const {
    return field;
}

std::ostream& operator <<(std::ostream &s, const FilterComparison &comparison) {
    s << "comparison" << std::endl;
    for (const FilterComparison::Instance &instance : comparison.instances) {
        s << " - " << comparison.field << " " << instance << std::endl;
    }
    return s;
}


std::ostream& operator <<(std::ostream &s, const FilterComparison::Instance &instance) {
    switch (instance.op) {
        case FilterComparison::Operator::Equal: s << "=="; break;
        case FilterComparison::Operator::NotEqual: s << "!="; break;
        case FilterComparison::Operator::Greater: s << ">"; break;
        case FilterComparison::Operator::GreaterEqual: s << ">="; break;
        case FilterComparison::Operator::Less: s << "<"; break;
        case FilterComparison::Operator::LessEqual: s << "<="; break;
        case FilterComparison::Operator::In: s << "in"; break;
        case FilterComparison::Operator::NotIn: s << "!in"; break;
    }

    s << " [ ";
    for (const Value &value : instance.values) {
        s << toString(value) << " ";
    }
    s << "]";
    return s;
}


FilterComparison::Operator parseFilterComparisonOperator(const std::string &op) {
    if (op == "==") return FilterComparison::Operator::Equal;
    if (op == "!=") return FilterComparison::Operator::NotEqual;
    if (op == ">") return FilterComparison::Operator::Greater;
    if (op == ">=") return FilterComparison::Operator::GreaterEqual;
    if (op == "<") return FilterComparison::Operator::Less;
    if (op == "<=") return FilterComparison::Operator::LessEqual;
    if (op == "in") return FilterComparison::Operator::In;
    if (op == "!in") return FilterComparison::Operator::NotIn;
    return FilterComparison::Operator::Equal;
}

template bool FilterComparison::compare(const VectorTileTagExtractor &extractor) const;

}
