#include <llmr/style/filter_expression_private.hpp>
#include <llmr/map/vector_tile.hpp>

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


std::ostream& operator <<(std::ostream &s, FilterExpression::Operator op) {
    switch (op) {
        case FilterExpression::Operator::And: s << "AND"; break;
        case FilterExpression::Operator::Or: s << "OR"; break;
        case FilterExpression::Operator::Xor: s << "XOR"; break;
        case FilterExpression::Operator::Nor: s << "NOR"; break;
    }
    return s;
}


std::ostream& operator <<(std::ostream &s, FilterExpression::GeometryType type) {
    switch (type) {
        case FilterExpression::GeometryType::Point: s << "Point"; break;
        case FilterExpression::GeometryType::LineString: s << "LineString"; break;
        case FilterExpression::GeometryType::Polygon: s << "Polygon"; break;
        case FilterExpression::GeometryType::Any: s << "<Any>"; break;
    }
    return s;
}

bool FilterExpression::empty() const {
    return type == GeometryType::Any && comparisons.empty() && expressions.empty();
}

void FilterExpression::add(const FilterComparison &comparison) {
    comparisons.emplace_back(comparison);
}

void FilterExpression::add(const FilterExpression &expression) {
    expressions.emplace_back(expression);
}

void FilterExpression::setGeometryType(GeometryType g) {
    type = g;
}

FilterExpression::GeometryType parseGeometryType(const std::string &geometry) {
    if (geometry == "Point") return FilterExpression::GeometryType::Point;
    if (geometry == "LineString") return FilterExpression::GeometryType::LineString;
    if (geometry == "Polygon") return FilterExpression::GeometryType::Polygon;
    return FilterExpression::GeometryType::Any;
}

std::ostream& operator <<(std::ostream &s, const FilterExpression &expression) {
    s << "expression " << expression.op << std::endl;
    s << " - $type = " << expression.type << std::endl;
    for (const FilterComparison &comparison : expression.comparisons) {
        s << comparison;
    }
    s << "end expression" << std::endl;
    return s;
}

// Explicit instantiation
template bool FilterComparison::compare(const VectorTileTagExtractor &extractor) const;
template bool FilterExpression::compare(const VectorTileTagExtractor &extractor) const;

}
