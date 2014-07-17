#ifndef LLMR_STYLE_FILTER_EXPRESSION
#define LLMR_STYLE_FILTER_EXPRESSION

#include <llmr/style/value.hpp>
#include <llmr/util/recursive_wrapper.hpp>

#include <vector>
#include <string>
#include <iosfwd>

namespace llmr {

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
        Instance(Operator op, std::vector<Value> &&values)
            : op(op), values(values) {}

        bool compare(const std::vector<Value> &property_values) const;

    private:
        Operator op = Operator::Equal;
        std::vector<Value> values;

        friend std::ostream& operator <<(std::ostream &, const Instance &);
    };

public:
    FilterComparison(const std::string &field) : field(field) {};

    const std::string &getField() const;
    template <typename Extractor> inline bool compare(const Extractor &extractor) const;

    template <typename ...Args>
    inline void add(Args&& ...args) {
        instances.emplace_back(::std::forward<Args>(args)...);
    }

private:
    std::string field;
    std::vector<Instance> instances;

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
