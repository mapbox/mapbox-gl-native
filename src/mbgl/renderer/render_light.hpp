#pragma once

#include <mbgl/style/light_impl.hpp>
#include <mbgl/renderer/transitioning_property.hpp>
#include <mbgl/renderer/cascade_parameters.hpp>
#include <mbgl/renderer/property_evaluator.hpp>
#include <mbgl/renderer/property_evaluation_parameters.hpp>
#include <mbgl/util/ignore.hpp>

#include <memory>

namespace mbgl {

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

using TransitioningLight = Transitioning<style::LightProperties>;
using EvaluatedLight     = Evaluated<style::LightProperties>;

class RenderLight {
public:
    RenderLight(Immutable<style::Light::Impl>);

    void transition(const CascadeParameters&);
    void evaluate(const PropertyEvaluationParameters&);
    bool hasTransition() const;

    const EvaluatedLight& getEvaluated() const;

    Immutable<style::Light::Impl> impl;

private:
    TransitioningLight transitioning;
    EvaluatedLight evaluated;
};

} // namespace mbgl
