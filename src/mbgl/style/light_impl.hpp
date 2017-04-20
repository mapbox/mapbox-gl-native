#pragma once

#include <mbgl/style/light.hpp>
#include <mbgl/style/transitioning_property.hpp>
#include <mbgl/style/cascade_parameters.hpp>
#include <mbgl/style/property_evaluator.hpp>
#include <mbgl/style/property_evaluation_parameters.hpp>
#include <mbgl/util/ignore.hpp>

namespace mbgl {
namespace style {

template <class TypeList>
class Transitioning;

template <class... Ps>
class Transitioning<TypeList<Ps...>> : public IndexedTuple<
    TypeList<Ps...>,
    TypeList<TransitioningProperty<typename Ps::ValueType>...>>
{
private:
    using Properties = TypeList<Ps...>;
    using Raw = IndexedTuple<Properties, Properties>;
    using Super = IndexedTuple<
        TypeList<Ps...>,
        TypeList<TransitioningProperty<typename Ps::ValueType>...>>;

public:
    Transitioning() = default;
    Transitioning(const Raw& raw, Transitioning&& prior, const CascadeParameters& params)
        : Super {
            TransitioningProperty<typename Ps::ValueType>(
                raw.template get<Ps>().value,
                std::move(prior.template get<Ps>()),
                raw.template get<Ps>().transition.reverseMerge(params.transition),
                params.now)...
        } {}

    bool hasTransition() const {
        bool result = false;
        util::ignore({ result |= this->template get<Ps>().hasTransition()... });
        return result;
    }
};

template <class TypeList>
class Evaluated;

template <class... Ps>
class Evaluated<TypeList<Ps...>> : public IndexedTuple<
    TypeList<Ps...>,
    TypeList<typename Ps::Type...>>
{
private:
    using Properties = TypeList<Ps...>;
    using TransitioningPs = Transitioning<Properties>;
    using Super = IndexedTuple<
        TypeList<Ps...>,
        TypeList<typename Ps::Type...>>;

public:
    Evaluated() = default;
    Evaluated(TransitioningPs& transitioning, const PropertyEvaluationParameters& params)
        : Super {
            transitioning.template get<Ps>()
                .evaluate(PropertyEvaluator<typename Ps::Type>(params, Ps::defaultValue()), params.now)...
        } {}
};

using TransitioningLight = Transitioning<LightProperties>;
using EvaluatedLight     = Evaluated<LightProperties>;

} // namespace style
} // namespace mbgl
