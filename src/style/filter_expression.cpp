#include <llmr/style/filter_expression.hpp>
#include <llmr/map/vector_tile.hpp>

namespace llmr {


template <typename Comparer>
inline bool FilterComparison::Instance::includes(const Value &property_value, const Comparer &comparer) const {
    for (const Value &filter_value : values) {
        if (comparer(property_value, filter_value)) {
            return true;
        }
    }
    return false;
}

template <typename Comparer>
inline bool FilterComparison::Instance::compare(const Value &property_value, const Comparer &comparer) const {
    for (const Value &filter_value : values) {
        if (!comparer(property_value, filter_value)) {
            return false;
        }
    }
    return true;
}

template <typename Comparer>
inline bool FilterComparison::Instance::all(const std::forward_list<Value> &property_values, const Comparer &comparer) const {
    for (const Value &property_value : property_values) {
        if (!compare(property_value, comparer)) {
            return false;
        }
    }
    return true;
}



bool FilterComparison::Instance::compare(const Value &property_value) const {
    switch (op) {
        case Operator::Equal:
        case Operator::In:
            return includes(property_value, util::relaxed_equal);
        case Operator::NotEqual:
        case Operator::NotIn:
            return !includes(property_value, util::relaxed_equal);
        case Operator::Greater:
            return compare(property_value, util::relaxed_greater);
        case Operator::GreaterEqual:
            return compare(property_value, util::relaxed_greater_equal);
        case Operator::Less:
            return compare(property_value, util::relaxed_less);
        case Operator::LessEqual:
            return compare(property_value, util::relaxed_less_equal);
        default:
            return false;
    }
}

bool FilterComparison::Instance::compare(const std::forward_list<Value> &property_values) const {
    switch (op) {
        case Operator::Equal:
            for (const Value &property_value : property_values) {
                if (!includes(property_value, util::relaxed_equal)) {
                    return false;
                }
            }
            return true;
        case Operator::NotEqual:
            for (const Value &property_value : property_values) {
                if (includes(property_value, util::relaxed_equal)) {
                    return false;
                }
            }
            return true;
        case Operator::In:
            for (const Value &property_value : property_values) {
                if (includes(property_value, util::relaxed_equal)) {
                    return true;
                }
            }
            return false;
        case Operator::NotIn:
            for (const Value &property_value : property_values) {
                if (!includes(property_value, util::relaxed_equal)) {
                    return true;
                }
            }
            return false;
        case Operator::Greater:
            return all(property_values, util::relaxed_greater);
        case Operator::GreaterEqual:
            return all(property_values, util::relaxed_greater_equal);
        case Operator::Less:
            return all(property_values, util::relaxed_less);
        case Operator::LessEqual:
            return all(property_values, util::relaxed_less_equal);
        default:
            return false;
    }
}


const std::string &FilterComparison::getField() const {
    return field;
}

inline bool FilterComparison::compare(const VectorTileTagExtractor &extractor) const {
    const std::forward_list<Value> values = extractor.getValues(field);

    // All instances are ANDed together.
    for (const Instance &instance : instances) {
        if (!instance.compare(values)) {
            return false;
        }
    }
    return true;
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



bool FilterExpression::compare(const VectorTileTagExtractor &extractor) const {
    if (type != GeometryType::Any && extractor.getType() != type && extractor.getType() != GeometryType::Any) {
        return false;
    }

    switch (op) {
    case Operator::And:
        for (const FilterComparison &comparison : comparisons) {
            if (!comparison.compare(extractor)) {
                return false;
            }
        }
        for (const FilterExpression &expression: expressions) {
            if (!expression.compare(extractor)) {
                return false;
            }
        }
        return true;
    case Operator::Or:
        for (const FilterComparison &comparison : comparisons) {
            if (comparison.compare(extractor)) {
                return true;
            }
        }
        for (const FilterExpression &expression: expressions) {
            if (expression.compare(extractor)) {
                return true;
            }
        }
        return false;
    case Operator::Xor: {
        int count = 0;
        for (const FilterComparison &comparison : comparisons) {
            count += comparison.compare(extractor);
            if (count > 1) {
                return false;
            }
        }
        for (const FilterExpression &expression: expressions) {
            count += expression.compare(extractor);
            if (count > 1) {
                return false;
            }
        }
        return count == 1;
    }
    case Operator::Nor:
        for (const FilterComparison &comparison : comparisons) {
            if (comparison.compare(extractor)) {
                return false;
            }
        }
        for (const FilterExpression &expression: expressions) {
            if (expression.compare(extractor)) {
                return false;
            }
        }
        return true;
    default:
        return true;
    }
}

bool FilterExpression::empty() const {
    return type == GeometryType::Any && comparisons.empty() && expressions.empty();
}

void FilterExpression::add(const FilterComparison &comparison) {
    comparisons.emplace_front(comparison);
}

void FilterExpression::add(const FilterExpression &expression) {
    expressions.emplace_front(expression);
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

}
