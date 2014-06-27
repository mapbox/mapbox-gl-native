#ifndef LLMR_STYLE_FILTER_EXPRESSION
#define LLMR_STYLE_FILTER_EXPRESSION

#include <llmr/style/value.hpp>
#include <llmr/util/recursive_wrapper.hpp>

#include <forward_list>
#include <string>
#include <ostream>
#include <iostream>

namespace llmr {

class VectorTileTagExtractor;

class FilterComparison {
public:
    enum class Operator : uint8_t {
        Equal,
        NotEqual,
        Greater,
        GreaterEqual,
        Less,
        LessEqual,
        In,
        NotIn
    };

    class Instance {
    public:
        Instance(Operator op, std::forward_list<Value> &&values)
            : op(op), values(values) {}

        bool compare(const Value &property_value) const;
        bool compare(const std::forward_list<Value> &property_values) const;

    private:
        template <typename Comparer> inline bool includes(const Value &property_value, const Comparer &comparer) const;
        template <typename Comparer> inline bool compare(const Value &property_value, const Comparer &comparer) const;
        template <typename Comparer> inline bool all(const std::forward_list<Value> &property_values, const Comparer &comparer) const;

    private:
        Operator op = Operator::Equal;
        std::forward_list<Value> values;

        friend std::ostream& operator <<(std::ostream &, const Instance &);
    };

public:
    FilterComparison(const std::string &field) : field(field) {};

    const std::string &getField() const;
    inline bool compare(const VectorTileTagExtractor &extractor) const;

    template <typename ...Args>
    inline void add(Args&& ...args) {
        instances.emplace_front(::std::forward<Args>(args)...);
    }

private:
    std::string field;
    std::forward_list<Instance> instances;

    friend std::ostream& operator <<(std::ostream &, const FilterComparison &);
};

std::ostream& operator <<(std::ostream &s, const FilterComparison &comparison);
std::ostream& operator <<(std::ostream &s, const FilterComparison::Instance &instance);


FilterComparison::Operator parseFilterComparisonOperator(const std::string &op);


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
        Line,
        Polygon
    };

public:
    FilterExpression() = default;
    FilterExpression(Operator op) : op(op) {};

    bool empty() const;

    bool compare(const VectorTileTagExtractor &extractor) const;
    void add(const FilterComparison &comparison);
    void add(const FilterExpression &expression);
    void setGeometryType(GeometryType g);

private:
    Operator op = Operator::And;
    GeometryType type = GeometryType::Any;
    std::forward_list<FilterComparison> comparisons;
    std::forward_list<FilterExpression::Wrapper> expressions;

    friend std::ostream& operator <<(std::ostream &, const FilterExpression &);
};

std::ostream& operator <<(std::ostream &s, const FilterExpression &expression);

FilterExpression::GeometryType parseGeometryType(const std::string &geometry);
}

#endif
