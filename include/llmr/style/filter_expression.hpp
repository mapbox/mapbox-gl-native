#ifndef LLMR_STYLE_FILTER_EXPRESSION
#define LLMR_STYLE_FILTER_EXPRESSION

#include <llmr/style/filter_comparison.hpp>
#include <llmr/util/recursive_wrapper.hpp>

#include <iosfwd>

namespace llmr {

class FilterExpression {
public:
    typedef util::recursive_wrapper<FilterExpression> Wrapper;

    enum class Operator : uint8_t {
        And,
        Or,
        Xor,
        Nor
    };

    enum class GeometryType : uint8_t {
        Any,
        Point,
        LineString,
        Polygon
    };

public:
    FilterExpression() = default;
    FilterExpression(Operator op) : op(op) {};

    bool empty() const;

    template <typename Extractor> bool compare(const Extractor &extractor) const;
    void add(const FilterComparison &comparison);
    void add(const FilterExpression &expression);
    void setGeometryType(GeometryType g);

private:
    Operator op = Operator::And;
    GeometryType type = GeometryType::Any;
    std::vector<FilterComparison> comparisons;
    std::vector<FilterExpression::Wrapper> expressions;

    friend std::ostream& operator <<(std::ostream &, const FilterExpression &);
};

std::ostream& operator <<(std::ostream &s, const FilterExpression &expression);

FilterExpression::GeometryType parseGeometryType(const std::string &geometry);
}

#endif
