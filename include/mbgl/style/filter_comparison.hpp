#ifndef MBGL_STYLE_FILTER_COMPARISON
#define MBGL_STYLE_FILTER_COMPARISON

#include <mbgl/style/value.hpp>

#include <vector>
#include <string>
#include <iosfwd>

namespace mbgl {

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

}

#endif
