#include <mapbox/optional.hpp>

#include <mbgl/style/value_comparison.hpp>

namespace mbgl {

template <class Extractor>
struct Evaluator : public mapbox::util::static_visitor<bool>
{
    const Extractor& extractor;

    Evaluator(const Extractor& extractor_)
        : extractor(extractor_) {}

    template <class E>
    bool operator()(const E& e) const { return e.evaluate(extractor); }
};

template <class Extractor>
bool evaluate(const FilterExpression& expression, const Extractor& extractor) {
    return mapbox::util::apply_visitor(Evaluator<Extractor>(extractor), expression);
};

template <class Extractor>
bool EqualsExpression::evaluate(const Extractor& extractor) const {
    mapbox::util::optional<Value> actual = extractor.getValue(key);
    return actual && util::relaxed_equal(*actual, value);
}

template <class Extractor>
bool NotEqualsExpression::evaluate(const Extractor& extractor) const {
    mapbox::util::optional<Value> actual = extractor.getValue(key);
    return !actual || util::relaxed_not_equal(*actual, value);
}

template <class Extractor>
bool LessThanExpression::evaluate(const Extractor& extractor) const {
    mapbox::util::optional<Value> actual = extractor.getValue(key);
    return actual && util::relaxed_less(*actual, value);
}

template <class Extractor>
bool LessThanEqualsExpression::evaluate(const Extractor& extractor) const {
    mapbox::util::optional<Value> actual = extractor.getValue(key);
    return actual && util::relaxed_less_equal(*actual, value);
}

template <class Extractor>
bool GreaterThanExpression::evaluate(const Extractor& extractor) const {
    mapbox::util::optional<Value> actual = extractor.getValue(key);
    return actual && util::relaxed_greater(*actual, value);
}

template <class Extractor>
bool GreaterThanEqualsExpression::evaluate(const Extractor& extractor) const {
    mapbox::util::optional<Value> actual = extractor.getValue(key);
    return actual && util::relaxed_greater_equal(*actual, value);
}

template <class Extractor>
bool InExpression::evaluate(const Extractor& extractor) const {
    mapbox::util::optional<Value> actual = extractor.getValue(key);
    if (!actual)
        return false;
    for (const auto& v: values) {
        if (util::relaxed_equal(*actual, v)) {
            return true;
        }
    }
    return false;
}

template <class Extractor>
bool NotInExpression::evaluate(const Extractor& extractor) const {
    mapbox::util::optional<Value> actual = extractor.getValue(key);
    if (!actual)
        return true;
    for (const auto& v: values) {
        if (util::relaxed_equal(*actual, v)) {
            return false;
        }
    }
    return true;
}

template <class Extractor>
bool AnyExpression::evaluate(const Extractor& extractor) const {
    Evaluator<Extractor> evaluator(extractor);
    for (const auto& e: expressions) {
        if (mapbox::util::apply_visitor(evaluator, e)) {
            return true;
        }
    }
    return false;
}

template <class Extractor>
bool AllExpression::evaluate(const Extractor& extractor) const {
    Evaluator<Extractor> evaluator(extractor);
    for (const auto& e: expressions) {
        if (!mapbox::util::apply_visitor(evaluator, e)) {
            return false;
        }
    }
    return true;
}

template <class Extractor>
bool NoneExpression::evaluate(const Extractor& extractor) const {
    Evaluator<Extractor> evaluator(extractor);
    for (const auto& e: expressions) {
        if (mapbox::util::apply_visitor(evaluator, e)) {
            return false;
        }
    }
    return true;
}

}
