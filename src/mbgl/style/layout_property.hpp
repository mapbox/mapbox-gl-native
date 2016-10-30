#pragma once

#include <mbgl/style/property_evaluator.hpp>
#include <mbgl/util/indexed_tuple.hpp>

namespace mbgl {
namespace style {

class PropertyEvaluationParameters;

template <class T>
class LayoutProperty {
public:
    using EvaluatorType = PropertyEvaluator<T>;
    using UnevaluatedType = PropertyValue<T>;
    using EvaluatedType = T;
};

template <class... Ps>
class LayoutProperties {
public:
    using Properties = TypeList<Ps...>;
    using EvaluatedTypes = TypeList<typename Ps::EvaluatedType...>;
    using UnevaluatedTypes = TypeList<typename Ps::UnevaluatedType...>;

    template <class TypeList>
    using Tuple = IndexedTuple<Properties, TypeList>;

    class Evaluated : public Tuple<EvaluatedTypes> {
    public:
        using Tuple<EvaluatedTypes>::Tuple;
    };

    class Unevaluated : public Tuple<UnevaluatedTypes> {
    public:
        using Tuple<UnevaluatedTypes>::Tuple;
    };

    template <class P>
    auto evaluate(const PropertyEvaluationParameters& parameters) const {
        using Evaluator = typename P::EvaluatorType;
        return unevaluated.template get<P>()
            .evaluate(Evaluator(parameters, P::defaultValue()));
    }

    Evaluated evaluate(const PropertyEvaluationParameters& parameters) const {
        return Evaluated {
            evaluate<Ps>(parameters)...
        };
    }

    Unevaluated unevaluated;
};

} // namespace style
} // namespace mbgl
