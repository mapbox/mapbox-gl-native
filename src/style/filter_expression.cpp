#include <mbgl/style/filter_expression_private.hpp>
#include <mbgl/map/vector_tile.hpp>

#include <mbgl/style/value_comparison.hpp>

#include <ostream>

namespace mbgl {

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

template bool FilterExpression::compare(const VectorTileTagExtractor &extractor) const;

}
